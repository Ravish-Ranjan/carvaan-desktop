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

        void createWindow(GtkApplication *app);

        std::string getFrontendPath() const;

        static void handleUriScheme(
            WebKitURISchemeRequest *request,
            gpointer user_data);

        void serveFrontendFile(WebKitURISchemeRequest *request);

        GtkApplication *application_;
        GtkWidget *window_;
        WebKitWebView *webview_;
        LibraryManager& libraryManager_;

        static void handleScriptMessage(WebKitUserContentManager* manager,WebKitJavascriptResult* result,gpointer user_data);
        void sendToReact(const std::string& message);
        void sendLibrarySongs();
        void handleLibraryImport(const std::string& message);
        void openImportFileChooser();
};
