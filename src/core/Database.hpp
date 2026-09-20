#pragma once

#include <sqlite3.h>

#include <string>

class Database {
    public:
        explicit Database(const std::string& path);
        ~Database();

        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;

        bool open();
        void close();

        bool initialize();

        sqlite3* handle() const;

    private:
        std::string path_;
        sqlite3* database_;
};