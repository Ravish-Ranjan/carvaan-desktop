#include "WebViewApp.hpp"
#include "Protocol.hpp"

#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

std::string env = "development";

WebViewApp::WebViewApp(LibraryManager& libraryManager)
    : libraryFeatures_(libraryManager,[this](const Json& message) {sendToReact(message);}),
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

    } else {
        g_warning("[Bridge] Unknown frontend event: %s",type.c_str());
    }
}


