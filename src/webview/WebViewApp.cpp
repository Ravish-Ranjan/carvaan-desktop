#include "WebViewApp.hpp"

#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

WebViewApp::WebViewApp(LibraryManager& libraryManager)
    : libraryManager_(libraryManager),
      application_(nullptr),
      window_(nullptr),
      webview_(nullptr) {}

int WebViewApp::run(int argc, char* argv[]) {
    application_ = gtk_application_new("org.ravishdev.syncsongsapp",G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(
        application_,
        "activate",
        G_CALLBACK(WebViewApp::activate),
        this);

    const int status = g_application_run(
        G_APPLICATION(application_),
        argc,
        argv);

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
    const std::filesystem::path frontendDir = installRoot / "share" / "sync-songs-app" / "frontend";
    const std::filesystem::path indexFile = frontendDir / "index.html";

    if (std::filesystem::exists(indexFile)) return indexFile.string();

    return {};
}

void WebViewApp::handleUriScheme(WebKitURISchemeRequest* request,gpointer user_data) {
    auto* self = static_cast<WebViewApp*>(user_data);
    self->serveFrontendFile(request);
}

void WebViewApp::serveFrontendFile(WebKitURISchemeRequest* request){
    const gchar* uri = webkit_uri_scheme_request_get_uri(request);

    if (uri == nullptr) {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new(G_IO_ERROR,G_IO_ERROR_INVALID_ARGUMENT,"Invalid URI")
        );
        return;
    }

    std::string uriString(uri);

    const std::string schemePrefix = "syncsongs://app";

    std::string relativePath;

    if (uriString == schemePrefix || uriString == schemePrefix + "/") {
        relativePath = "index.html";
    } else if (uriString.rfind(schemePrefix + "/", 0) == 0) {
        relativePath = uriString.substr(schemePrefix.size() + 1 );
    } else {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_NOT_FOUND,
                "Invalid application URI"
            )
        );
        return;
    }

    // Prevent directory traversal.
    const std::filesystem::path frontendDir = std::filesystem::canonical(getFrontendPath()).parent_path();
    const std::filesystem::path requestedPath = frontendDir / relativePath;
    std::error_code ec;
    const std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(requestedPath, ec);

    if (ec || canonicalPath.string().rfind(frontendDir.string(),0) != 0) {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_PERMISSION_DENIED,
                "Invalid frontend path"
            )
        );
        return;
    }

    if (!std::filesystem::is_regular_file(canonicalPath)) {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_NOT_FOUND,
                "Frontend file not found"
            )
        );
        return;
    }

    std::ifstream file(canonicalPath, std::ios::binary);

    if (!file) {
        webkit_uri_scheme_request_finish_error(
            request,
            g_error_new(
                G_IO_ERROR,
                G_IO_ERROR_FAILED,
                "Could not open frontend file"
            )
        );
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    const std::string contents = buffer.str();

    GInputStream* stream =
        g_memory_input_stream_new_from_data(
            g_memdup2(contents.data(), contents.size()),
            contents.size(),
            g_free
        );

    std::string mimeType = "application/octet-stream";

    const std::string extension = canonicalPath.extension().string();

    if (extension == ".html") {
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

    webkit_uri_scheme_request_finish(request,stream,contents.size(),mimeType.c_str());
    g_object_unref(stream);
}

void WebViewApp::handleScriptMessage(WebKitUserContentManager* manager,WebKitJavascriptResult* result,gpointer user_data) {
    (void)manager;

    auto* self = static_cast<WebViewApp*>(user_data);
    JSCValue* value = webkit_javascript_result_get_js_value(result);
    gchar* message = jsc_value_to_string(value);

    if (message != nullptr) {
        g_print("React -> C++: %s\n", message);

        if (std::string(message).find(R"("type":"library-import-files")") != std::string::npos) {
            self->handleLibraryImport(message);
        } else if (std::string(message).find(R"("type":"open-import-dialog")") != std::string::npos) {
            self->openImportFileChooser();
        }

        if (std::string(message).find("frontend-ready") !=std::string::npos) {
            self->sendToReact(R"({"type":"test","message":"Hello from C++"})");
        }

        g_free(message);
    }

    webkit_javascript_result_unref(result);
}

void WebViewApp::sendToReact(const std::string& message){
    if (webview_ == nullptr) return;
    std::string script = "window.dispatchEvent(new CustomEvent('syncsongs-message', { detail: " + message + " }))";
    webkit_web_view_run_javascript(webview_,script.c_str(),nullptr,nullptr,nullptr);
}

void WebViewApp::createWindow(GtkApplication* app){
    window_ = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window_),"Sync Songs");
    gtk_window_set_default_size(GTK_WINDOW(window_),1000,600);

    WebKitWebContext* context = webkit_web_context_get_default();

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
    
    webkit_web_view_load_uri(webview_,"syncsongs://app/index.html");

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

void WebViewApp::handleLibraryImport(const std::string& message) {
    const std::string prefix = R"({"type":"library-import-files","files":[)";
    if (message.rfind(prefix, 0) != 0) return;

    std::string filesJson = message.substr(prefix.size());
    if (!filesJson.empty() && filesJson.back() == '}') filesJson.pop_back();

    std::vector<std::filesystem::path> files;
    std::size_t position = 0;

    while (position < filesJson.size()) {
        const std::size_t start = filesJson.find('"', position);
        if (start == std::string::npos) break;

        const std::size_t end = filesJson.find('"', start + 1);
        if (end == std::string::npos) break;

        files.emplace_back(filesJson.substr(start + 1,end - start - 1));
        position = end + 1;
    }

    const std::size_t imported = libraryManager_.importFiles(files);
    g_print("Imported %zu song(s)\n",imported);
}

void WebViewApp::openImportFileChooser() {
    GtkWidget* dialog =
        gtk_file_chooser_dialog_new(
            "Import Songs",
            GTK_WINDOW(window_),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            "_Cancel",
            GTK_RESPONSE_CANCEL,
            "_Open",
            GTK_RESPONSE_ACCEPT,
            nullptr
        );

    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),TRUE);

    GtkFileFilter* audioFilter = gtk_file_filter_new();
    gtk_file_filter_set_name(audioFilter,"Audio Files");
    gtk_file_filter_add_mime_type(audioFilter,"audio/mpeg");
    gtk_file_filter_add_mime_type(audioFilter,"audio/mp4");
    gtk_file_filter_add_mime_type(audioFilter,"audio/flac");
    gtk_file_filter_add_mime_type(audioFilter,"audio/wav");
    gtk_file_filter_add_mime_type(audioFilter,"audio/ogg");
    gtk_file_filter_add_mime_type(audioFilter,"audio/opus");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),audioFilter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GSList* selectedFiles = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));

        std::vector<std::filesystem::path> files;

        for (GSList* item = selectedFiles;item != nullptr;item = item->next) {
            const char* filename = static_cast<const char*>(item->data);
            if (filename != nullptr) files.emplace_back(filename);
            g_free(item->data);
        }

        g_slist_free(selectedFiles);
        const std::size_t imported = libraryManager_.importFiles(files);
        g_print("Imported %zu song(s)\n",imported);
        sendToReact("{\"type\":\"library-import-result\",\"imported\":" +std::to_string(imported) +"}");
    }

    gtk_widget_destroy(dialog);
}