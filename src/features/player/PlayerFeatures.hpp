#pragma once

#include "../../core/LibraryManager.hpp"
#include <functional>
#include <string>

class PlayerFeatures {
    public:
        using SendToReact = std::function<void(const Json&)>;
        using MediaUrlProvider = std::function<std::string(const std::string&)>;

        PlayerFeatures(LibraryManager& libraryManager,SendToReact sendToReact,MediaUrlProvider mediaUrlProvider);

        void playSong(const Json& message);

    private:
        LibraryManager& libraryManager_;
        SendToReact sendToReact_;
        MediaUrlProvider mediaUrlProvider_;
};