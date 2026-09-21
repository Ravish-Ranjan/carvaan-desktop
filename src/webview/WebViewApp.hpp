#pragma once

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include "../core/LibraryManager.hpp" 

#include <string>

class WebViewApp {
    public:
        explicit WebViewApp(LibraryManager& libraryManager);
        int run(int argc, char *argv[]);

    private:
        static void activate(GtkApplication *app, gpointer user_data);
        static void handleUriScheme(WebKitURISchemeRequest *request,gpointer user_data);
        static void handleScriptMessage(WebKitUserContentManager* manager,WebKitJavascriptResult* result,gpointer user_data);

        
        GtkApplication *application_;
        GtkWidget *window_;
        WebKitWebView *webview_;
        LibraryManager& libraryManager_;

        std::string getFrontendPath() const;
        
        void createWindow(GtkApplication *app);
        void serveFrontendFile(WebKitURISchemeRequest *request);
        void sendLibrarySongs();
        void openImportFileChooser();
        void handleFrontendMessage(const Json& message);
        void handleLibraryImport(const Json& message);
        void handleFrontendReady(const Json& message);
        void sendToReact(const Json& message);
};
