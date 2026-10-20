#pragma once

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <libsoup/soup.h>

#include "../core/LibraryManager.hpp" 
#include "../features/library/LibraryFeatures.hpp"
#include "../features/configure/ConfigureFeatures.hpp"
#include "../features/player/PlayerFeatures.hpp"

#include <string>

class WebViewApp {
    public:
        explicit WebViewApp(LibraryManager& libraryManager,SettingsRepository& settingsRepository);
        int run(int argc, char *argv[]);

    private:
        static void activate(GtkApplication *app, gpointer user_data);
        static void handleUriScheme(WebKitURISchemeRequest *request,gpointer user_data);
        static void handleScriptMessage(WebKitUserContentManager* manager,WebKitJavascriptResult* result,gpointer user_data);

        
        GtkApplication *application_;
        GtkWidget *window_;
        WebKitWebView *webview_;
        LibraryFeatures libraryFeatures_;
        ConfigureFeatures configureFeatures_;
        PlayerFeatures playerFeatures_;
        SoupServer* mediaServer_;

        guint mediaServerPort_;
        
        bool startMediaServer();
        void stopMediaServer();
        static void handleMediaRequest(
            SoupServer* server,
            SoupServerMessage* message,
            const char* path,
            GHashTable* query,
            gpointer user_data
        );

        std::string getFrontendPath() const;
        
        void createWindow(GtkApplication *app);
        void serveFrontendFile(WebKitURISchemeRequest *request);
        void handleFrontendMessage(const Json& message);
        void sendToReact(const Json& message);
};
