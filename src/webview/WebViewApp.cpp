#include "WebViewApp.hpp"
#include "Protocol.hpp"
#include "../core/AppPaths.hpp"
#include <glib.h>
#include <libsoup/soup.h>

#include <algorithm>
#include <cctype>
#include <limits>
#include <memory>
#include <optional>
#include <string_view>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

std::string env = "development";

namespace {
    void addCorsHeaders(SoupServerMessage* message) {
        SoupMessageHeaders* headers = soup_server_message_get_response_headers(message);
        soup_message_headers_replace(headers, "Access-Control-Allow-Origin", "*");
        soup_message_headers_replace(headers, "Access-Control-Allow-Methods", "GET, HEAD, OPTIONS");
        soup_message_headers_replace(headers, "Access-Control-Allow-Headers", "Range, Content-Type");
        soup_message_headers_replace(headers, "Access-Control-Expose-Headers","Accept-Ranges, Content-Range, Content-Length");
        soup_message_headers_replace(headers, "Accept-Ranges", "bytes");
    }

    std::string getMediaMimeType(const std::filesystem::path& path) {
        std::string extension = path.extension().string();

        std::transform(extension.begin(),extension.end(),extension.begin(),[](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (extension == ".mp3") return "audio/mpeg";
        if (extension == ".wav") return "audio/wav";
        if (extension == ".ogg" || extension == ".oga" || extension == ".opus") return "audio/ogg";
        if (extension == ".flac") return "audio/flac";
        if (extension == ".m4a" || extension == ".mp4") return "audio/mp4";
        if (extension == ".aac") return "audio/aac";
        if (extension == ".webm") return "audio/webm";

        return "application/octet-stream";
    }

    bool parseByteRange(const char* header,std::uint64_t fileSize,std::uint64_t& start,std::uint64_t& end) {
        if (header == nullptr || fileSize == 0) return false;

        const std::string value(header);
        if (value.rfind("bytes=", 0) != 0) return false;

        const std::string range = value.substr(6);
        if (range.find(',') != std::string::npos) return false;

        const auto dash = range.find('-');
        if (dash == std::string::npos) return false;

        const std::string first = range.substr(0, dash);
        const std::string last = range.substr(dash + 1);

        auto parseNumber = [](const std::string& text,std::uint64_t& result) -> bool {
            if (text.empty()) return false;

            const char* begin = text.data();
            const char* finish = begin + text.size();

            auto [ptr, error] = std::from_chars(begin, finish, result);

            return error == std::errc{} && ptr == finish;
        };

        if (first.empty()) {
            std::uint64_t suffixLength = 0;

            if (!parseNumber(last, suffixLength) || suffixLength == 0) return false;

            const std::uint64_t length = std::min(suffixLength, fileSize);

            start = fileSize - length;
            end = fileSize - 1;

            return true;
        }

        if (!parseNumber(first, start) || start >= fileSize) return false;

        if (last.empty()) {
            end = fileSize - 1;
            return true;
        }

        if (!parseNumber(last, end) || end < start) return false;

        end = std::min(end, fileSize - 1);
        return true;
    }
} 

WebViewApp::WebViewApp(LibraryManager& libraryManager,SettingsRepository& settingsRepository)
    : application_(nullptr),
      window_(nullptr),
      webview_(nullptr),
      mediaServer_(nullptr),
      mediaServerPort_(0),
      libraryFeatures_(libraryManager,[this](const Json& message) {sendToReact(message);}),
      configureFeatures_(settingsRepository,[this](const Json& message) {sendToReact(message);}),
      playerFeatures_(
        libraryManager,
        [this](const Json& message) {sendToReact(message);},
        [this](const std::string& filename) -> std::string {
              if (mediaServerPort_ == 0) {
                  g_warning("[Player] Media server is not running");
                  return "";
              }

              gchar* encodedFilename = g_uri_escape_string(filename.c_str(),nullptr,TRUE);

              if (encodedFilename == nullptr) return "";

              std::string url = "http://127.0.0.1:" + std::to_string(mediaServerPort_) + "/media/" + encodedFilename;

              g_free(encodedFilename);
              return url;
          }
      ) {}

int WebViewApp::run(int argc, char* argv[]) {
    application_ = gtk_application_new("org.ravishdev.carvaan-desktop",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application_,"activate",G_CALLBACK(WebViewApp::activate),this);

    const int status = g_application_run(G_APPLICATION(application_),argc,argv);

    stopMediaServer();

    g_object_unref(application_);
    application_ = nullptr;

    return status;
}

void WebViewApp::activate(GtkApplication* app, gpointer user_data) {
    auto* self = static_cast<WebViewApp*>(user_data);
    self->createWindow(app);
}

std::string WebViewApp::getFrontendPath() const{
    const std::filesystem::path executable = std::filesystem::canonical("/proc/self/exe");
    const std::filesystem::path installRoot = executable.parent_path().parent_path();
    const std::filesystem::path frontendDir = installRoot / "share" / "carvaan-desktop" / "frontend";
    const std::filesystem::path indexFile = frontendDir / "index.html";

    if (std::filesystem::exists(indexFile)) return indexFile.string();

    return {};
}

void WebViewApp::handleUriScheme(WebKitURISchemeRequest* request,gpointer user_data) {
    auto* self = static_cast<WebViewApp*>(user_data);
    self->serveFrontendFile(request);
}

void WebViewApp::serveFrontendFile(WebKitURISchemeRequest* request) {
    const gchar* uri = webkit_uri_scheme_request_get_uri(request);

    auto finishError = [&](GIOErrorEnum code, const char* message) {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new_literal(
                G_IO_ERROR,
                code,
                message
            )
        );
    };

    if (uri == nullptr) {
        finishError(G_IO_ERROR_INVALID_ARGUMENT, "Invalid URI");
        return;
    }

    const std::string uriString(uri);
    std::filesystem::path requestedPath;
    bool isMedia = false;

    // Handle temporary media files.
    const std::string mediaPrefix = "syncsongs://media/";

    if (uriString.rfind(mediaPrefix, 0) == 0) {
        isMedia = true;

        const std::string encodedFilename = uriString.substr(mediaPrefix.size());
        // Media URLs must identify exactly one filename.
        if (encodedFilename.empty() ||
            encodedFilename.find('/') != std::string::npos ||
            encodedFilename.find('\\') != std::string::npos ||
            encodedFilename.find('?') != std::string::npos ||
            encodedFilename.find('#') != std::string::npos) {
            finishError(
                G_IO_ERROR_INVALID_ARGUMENT,
                "Invalid media filename"
            );
            return;
        }

        gchar* decodedFilename = g_uri_unescape_string(encodedFilename.c_str(),nullptr);

        if (decodedFilename == nullptr) {
            finishError(G_IO_ERROR_INVALID_ARGUMENT,"Invalid encoded media filename");
            return;
        }

        const std::filesystem::path filename(decodedFilename);
        g_free(decodedFilename);

        if (filename.empty() ||
            filename.has_parent_path() ||
            filename == "." ||
            filename == "..") {
            finishError(
                G_IO_ERROR_PERMISSION_DENIED,
                "Invalid media path"
            );
            return;
        }

        const std::filesystem::path mediaDir = AppPaths::tempSongsDirectory();

        if (mediaDir.empty()) {
            finishError(G_IO_ERROR_NOT_FOUND,"Media directory unavailable");
            return;
        }

        std::error_code ec;
        const auto canonicalDir = std::filesystem::canonical(mediaDir, ec);

        if (ec) {
            finishError(G_IO_ERROR_NOT_FOUND,"Media directory not found");
            return;
        }

        const auto canonicalFile = std::filesystem::canonical(canonicalDir / filename, ec);

        if (ec || canonicalFile.parent_path() != canonicalDir) {
            finishError(G_IO_ERROR_PERMISSION_DENIED,"Invalid media path");
            return;
        }

        if (!std::filesystem::is_regular_file(canonicalFile, ec) || ec) {
            finishError(G_IO_ERROR_NOT_FOUND,"Media file not found");
            return;
        }

        requestedPath = canonicalFile;
    } else {
        const std::string appPrefix = "syncsongs://app";
        std::string relativePath;

        if (uriString == appPrefix ||
            uriString == appPrefix + "/") {
            relativePath = "index.html";
        } else if (uriString.rfind(appPrefix + "/", 0) == 0) {
            relativePath = uriString.substr(appPrefix.size() + 1);
        } else {
            finishError(G_IO_ERROR_NOT_FOUND,"Invalid application URI");
            return;
        }

        const std::filesystem::path indexPath = getFrontendPath();

        if (indexPath.empty()) {
            finishError(G_IO_ERROR_NOT_FOUND,"Frontend directory unavailable");
            return;
        }

        std::error_code ec;
        const auto frontendDir =
            std::filesystem::canonical(indexPath, ec).parent_path();

        if (ec) {
            finishError(G_IO_ERROR_NOT_FOUND,"Frontend directory not found");
            return;
        }

        gchar* decodedPath = g_uri_unescape_string(relativePath.c_str(),nullptr);
        
        if (decodedPath == nullptr) {
            finishError(G_IO_ERROR_INVALID_ARGUMENT,"Invalid encoded frontend path");
            return;
        }
        
        const std::filesystem::path decodedRelativePath(decodedPath);
        g_free(decodedPath);

        const auto canonicalFile = std::filesystem::canonical(frontendDir / decodedRelativePath,ec);

        if (ec) {
            finishError(G_IO_ERROR_NOT_FOUND,"Frontend file not found");
            return;
        }

        // Compare path components rather than string prefixes.
        auto dirIt = frontendDir.begin();
        auto fileIt = canonicalFile.begin();

        for (; dirIt != frontendDir.end() &&
               fileIt != canonicalFile.end();
             ++dirIt, ++fileIt) {
            if (*dirIt != *fileIt) {
                break;
            }
        }

        if (dirIt != frontendDir.end()) {
            finishError(
                G_IO_ERROR_PERMISSION_DENIED,
                "Invalid frontend path"
            );
            return;
        }

        requestedPath = canonicalFile;
    }

    std::ifstream file(requestedPath, std::ios::binary);

    if (!file) {
        finishError(
            G_IO_ERROR_FAILED,
            "Could not open requested file"
        );
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    const std::string contents = buffer.str();

    GInputStream* stream = g_memory_input_stream_new_from_data(
        g_memdup2(contents.data(), contents.size()),
        contents.size(),
        g_free
    );

    const std::string extension = requestedPath.extension().string();

    std::string mimeType = "application/octet-stream";

    if (extension == ".mp3") {
        mimeType = "audio/mpeg";
    } else if (extension == ".wav") {
        mimeType = "audio/wav";
    } else if (extension == ".ogg" || extension == ".oga") {
        mimeType = "audio/ogg";
    } else if (extension == ".opus") {
        mimeType = "audio/ogg";
    } else if (extension == ".flac") {
        mimeType = "audio/flac";
    } else if (extension == ".m4a" || extension == ".mp4") {
        mimeType = "audio/mp4";
    } else if (extension == ".aac") {
        mimeType = "audio/aac";
    } else if (extension == ".webm") {
        mimeType = "audio/webm";
    } else if (extension == ".html") {
        mimeType = "text/html";
    } else if (extension == ".js") {
        mimeType = "text/javascript";
    } else if (extension == ".css") {
        mimeType = "text/css";
    } else if (extension == ".svg") {
        mimeType = "image/svg+xml";
    } else if (extension == ".png") {
        mimeType = "image/png";
    } else if (extension == ".jpg" || extension == ".jpeg") {
        mimeType = "image/jpeg";
    } else if (extension == ".woff") {
        mimeType = "font/woff";
    } else if (extension == ".woff2") {
        mimeType = "font/woff2";
    }

    webkit_uri_scheme_request_finish(
        request,
        stream,
        static_cast<gssize>(contents.size()),
        mimeType.c_str()
    );

    g_object_unref(stream);
}


bool matchEvent(gchar* message,std::string event_type) {
    return std::string(message).find(event_type) != std::string::npos;
}

void WebViewApp::handleScriptMessage(
    WebKitUserContentManager* manager,
    WebKitJavascriptResult* result,
    gpointer user_data
) {
    (void)manager;

    auto* self = static_cast<WebViewApp*>(user_data);
    JSCValue* value = webkit_javascript_result_get_js_value(result);
    gchar* message = jsc_value_to_string(value);

    if (message == nullptr) {
        g_warning("[Bridge] Received null message from React");
        webkit_javascript_result_unref(result);
        return;
    }

    try {
        const Json event = Json::parse(message);

        if (!event.contains("type")) {
            g_warning("[Bridge] React message has no type");
        } else if (!event.at("type").is_string()) {
            g_warning("[Bridge] React message 'type' is not a string: %s",
                Json::stringify(event.at("type")).c_str()
            );
        } else {
            self->handleFrontendMessage(event);
        }

    } catch (const Json::exception& error) {
        g_warning("[Bridge] Invalid Json message from React: %s",error.what());
    }

    g_free(message);
    webkit_javascript_result_unref(result);
}

void WebViewApp::sendToReact(const Json& message){
    if (webview_ == nullptr) return;

    std::string json = Json::stringify(message);

    std::string script = 
        "window.dispatchEvent("
        "new CustomEvent('syncsongs-message', "
        "{ detail: " + json + " })"
        ")";
        
    webkit_web_view_evaluate_javascript(
        webview_,
        script.c_str(),
        -1,
        nullptr,nullptr,nullptr,nullptr,nullptr
    );
}

void WebViewApp::createWindow(GtkApplication* app){
    if (!startMediaServer()) {
        g_warning("[MediaServer] Failed to start media server");
    }
    window_ = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_),"Sync Songs");
    gtk_window_set_default_size(GTK_WINDOW(window_),1000,600);

    WebKitWebContext* context = webkit_web_context_get_default();

    WebKitSecurityManager* securityManager = webkit_web_context_get_security_manager(context);
    webkit_security_manager_register_uri_scheme_as_cors_enabled(securityManager,"syncsongs");
    
    webkit_web_context_register_uri_scheme(
        context,
        "syncsongs",
        WebViewApp::handleUriScheme,
        this,
        nullptr
    );

    webview_ = WEBKIT_WEB_VIEW(webkit_web_view_new());

    WebKitSettings* settings = webkit_web_view_get_settings(webview_);
    
    webkit_settings_set_enable_developer_extras(settings, TRUE);

    WebKitUserContentManager* contentManager = webkit_web_view_get_user_content_manager(webview_);
    webkit_user_content_manager_register_script_message_handler(contentManager,"syncSongs");
    
    g_signal_connect(
        contentManager,
        "script-message-received::syncSongs",
        G_CALLBACK(WebViewApp::handleScriptMessage),
        this
    );
    
    if (env == "production") {
        webkit_web_view_load_uri(webview_,"syncsongs://app/index.html");
    } else {
        webkit_web_view_load_uri(webview_,"http://localhost:5173");
    }

    g_signal_connect(webview_,"load-changed",
        G_CALLBACK(+[](
            WebKitWebView* view,
            WebKitLoadEvent loadEvent,
            gpointer user_data
        ) {
            if (loadEvent == WEBKIT_LOAD_FINISHED) {
                auto* self = static_cast<WebViewApp*>(user_data);
                self->sendToReact(R"({"type":"test","message":"Hello from C++"})");
            }
        }),
        this
    );

    gtk_container_add(GTK_CONTAINER(window_),GTK_WIDGET(webview_));
    gtk_widget_show_all(window_);
}

void WebViewApp::handleFrontendMessage(const Json& message) {
    if (!message.contains("type")) {
        g_warning("[Bridge] Frontend message has no type");
        return;
    }

    const Json& typeJson = message.at("type");
    const std::string type = typeJson.get<std::string>();

    g_print("[Bridge] %s\n",type.c_str());

    if (type == Protocol::Frontend::READY) {
        (void)message;
        g_print("[Bridge] Frontend is ready\n");
        
    } else if (type == Protocol::Frontend::GET_LIBRARY) {
        libraryFeatures_.sendLibrarySongs();
        
    } else if (type == Protocol::Frontend::OPEN_IMPORT_DIALOG) {
        libraryFeatures_.openImportFileChooser(GTK_WINDOW(window_));
        
    } else if (type == Protocol::Frontend::LIBRARY_IMPORT_FILES) {
        libraryFeatures_.handleLibraryImport(message);

    } else if (type == Protocol::Frontend::GET_COMBINED_SIZE) {
        libraryFeatures_.getCombinedSongsSize();
        
    } else if (type == Protocol::Frontend::OPEN_NEW_SONGS_IN_PICARD) {
        libraryFeatures_.openNewSongsInPicard();

    } else if (type == Protocol::Frontend::OPEN_PC_MUSIC_DIRECTORY_DIALOG) {
        configureFeatures_.openPcMusicDirectoryDialog(GTK_WINDOW(window_));
        
    } else if (type == Protocol::Frontend::GET_SETTINGS) {
        configureFeatures_.getSettings();
        
    } else if (type == Protocol::Frontend::PLAY_SONG) {
        playerFeatures_.playSong(message);
        
    } else if (type == Protocol::Frontend::SET_PC_MUSIC_DESTINATION) {
        configureFeatures_.setPcMusicDestination(message);

    } else if (type == Protocol::Frontend::SET_PHONE_DESTINATION) {
        configureFeatures_.setPhoneDestination(message);

    } else {
        g_warning("[Bridge] Unknown frontend event: %s",type.c_str());
    }
}

void WebViewApp::handleMediaRequest(SoupServer* server,SoupServerMessage* message,const char* path,GHashTable* query,gpointer user_data) {
    (void)server;
    (void)query;

    auto* self = static_cast<WebViewApp*>(user_data);
    (void)self;

    addCorsHeaders(message);

    const char* method = soup_server_message_get_method(message);

    if (method == nullptr) {
        soup_server_message_set_status(message, SOUP_STATUS_BAD_REQUEST, nullptr);
        return;
    }

    if (g_strcmp0(method, "OPTIONS") == 0) {
        soup_server_message_set_status(message, SOUP_STATUS_NO_CONTENT, nullptr);
        return;
    }

    if (g_strcmp0(method, "GET") != 0 && g_strcmp0(method, "HEAD") != 0) {
        soup_server_message_set_status(message, SOUP_STATUS_METHOD_NOT_ALLOWED, nullptr);
        return;
    }

    constexpr const char* prefix = "/media/";

    if (path == nullptr || std::string(path).rfind(prefix, 0) != 0) {
        soup_server_message_set_status(message, SOUP_STATUS_NOT_FOUND, nullptr);
        return;
    }

    const std::string filename = std::string(path).substr(std::char_traits<char>::length(prefix));

    if (filename.empty() ||
        filename == "." || filename == ".." ||
        filename.find('/') != std::string::npos ||
        filename.find('\\') != std::string::npos ||
        filename.find('\0') != std::string::npos
    ) {
        soup_server_message_set_status(message, SOUP_STATUS_BAD_REQUEST, nullptr);
        return;
    }

    const std::filesystem::path mediaDirectory = AppPaths::tempSongsDirectory();

    if (mediaDirectory.empty()) {
        soup_server_message_set_status(message, SOUP_STATUS_INTERNAL_SERVER_ERROR, nullptr);
        return;
    }

    std::error_code ec;

    const auto canonicalDirectory = std::filesystem::canonical(mediaDirectory, ec);

    if (ec) {
        soup_server_message_set_status(message, SOUP_STATUS_NOT_FOUND, nullptr);
        return;
    }

    const auto canonicalFile = std::filesystem::canonical(canonicalDirectory / filename, ec);

    // Check the canonical parent to prevent traversal and symlink escapes.
    if (ec || canonicalFile.parent_path() != canonicalDirectory || !std::filesystem::is_regular_file(canonicalFile, ec) || ec) {
        soup_server_message_set_status(message, SOUP_STATUS_NOT_FOUND, nullptr);
        return;
    }

    const std::uint64_t fileSize = std::filesystem::file_size(canonicalFile, ec);

    if (ec || fileSize > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        soup_server_message_set_status(message, SOUP_STATUS_INTERNAL_SERVER_ERROR, nullptr);
        return;
    }

    const std::string mimeType = getMediaMimeType(canonicalFile);

    SoupMessageHeaders* responseHeaders = soup_server_message_get_response_headers(message);

    soup_message_headers_set_content_type(responseHeaders, mimeType.c_str(), nullptr);

    // HEAD returns the same headers as GET but no body.
    if (g_strcmp0(method, "HEAD") == 0) {
        soup_message_headers_set_content_length(responseHeaders, fileSize);
        soup_server_message_set_status(message, SOUP_STATUS_OK, nullptr);
        return;
    }

    std::uint64_t start = 0;
    std::uint64_t end = fileSize == 0 ? 0 : fileSize - 1;

    const bool hasRange = soup_message_headers_get_one(soup_server_message_get_request_headers(message),"Range") != nullptr;

    if (hasRange) {
        const char* rangeHeader = soup_message_headers_get_one(soup_server_message_get_request_headers(message),"Range");

        if (!parseByteRange(rangeHeader, fileSize, start, end)) {
            soup_message_headers_replace(responseHeaders,"Content-Range",("bytes */" + std::to_string(fileSize)).c_str());
            soup_server_message_set_status(message, SOUP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE,nullptr);
            return;
        }
    }

    std::ifstream file(canonicalFile, std::ios::binary);

    if (!file) {
        soup_server_message_set_status(message, SOUP_STATUS_INTERNAL_SERVER_ERROR, nullptr);
        return;
    }

    const std::uint64_t contentLength = fileSize == 0 ? 0 : end - start + 1;

    std::string body(static_cast<std::size_t>(contentLength), '\0');

    if (contentLength > 0) {
        file.seekg(static_cast<std::streamoff>(start));

        file.read(body.data(),static_cast<std::streamsize>(contentLength));
        if (file.gcount() != static_cast<std::streamsize>(contentLength)) {
            soup_server_message_set_status(message, SOUP_STATUS_INTERNAL_SERVER_ERROR, nullptr);
            return;
        }
    }

    if (hasRange) {
        soup_message_headers_replace(responseHeaders,"Content-Range",
            ("bytes " + std::to_string(start) + "-" +
             std::to_string(end) + "/" +
             std::to_string(fileSize)).c_str()
        );

        soup_server_message_set_status(message, SOUP_STATUS_PARTIAL_CONTENT, nullptr);
    } else {
        soup_server_message_set_status(message, SOUP_STATUS_OK, nullptr);
    }

    soup_server_message_set_response(message,mimeType.c_str(),SOUP_MEMORY_COPY,body.data(),body.size());
}

bool WebViewApp::startMediaServer() {
    if (mediaServer_ != nullptr) return true;

    mediaServer_ = soup_server_new("server-header", "SyncSongsMedia",nullptr);

    soup_server_add_handler(mediaServer_,"/media",WebViewApp::handleMediaRequest,this,nullptr);

    GError* error = nullptr;

    if (!soup_server_listen_local(mediaServer_,0,SOUP_SERVER_LISTEN_IPV4_ONLY,&error)) {
        g_warning("[Media] Failed to start HTTP server: %s",error != nullptr ? error->message : "Unknown error");
        g_clear_error(&error);
        g_clear_object(&mediaServer_);
        return false;
    }

    GSList* uris = soup_server_get_uris(mediaServer_);

    if (uris == nullptr) {
        g_warning("[Media] Server started but no listening URI was returned");
        stopMediaServer();
        return false;
    }

    auto* uri = static_cast<GUri*>(uris->data);

    mediaServerPort_ = static_cast<guint>(g_uri_get_port(uri));

    g_slist_free_full(uris,reinterpret_cast<GDestroyNotify>(g_uri_unref));

    if (mediaServerPort_ == 0) {
        g_warning("[Media] Could not determine the server port");
        stopMediaServer();
        return false;
    }

    g_print("[Media] HTTP server listening on 127.0.0.1:%u\n",mediaServerPort_);

    return true;
}

void WebViewApp::stopMediaServer() {
    if (mediaServer_ != nullptr) {
        soup_server_disconnect(mediaServer_);
        g_clear_object(&mediaServer_);
    }

    mediaServerPort_ = 0;
}

