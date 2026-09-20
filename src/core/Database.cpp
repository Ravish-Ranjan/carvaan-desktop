#include "Database.hpp"

Database::Database(const std::string& path): path_(path),database_(nullptr){}

Database::~Database() {
    close();
}

bool Database::open() {
    if (database_ != nullptr) return true;

    const int result = sqlite3_open(path_.c_str(),&database_);

    if (result != SQLITE_OK) {
        close();
        return false;
    }

    return true;
}

void Database::close() {
    if (database_ != nullptr) {
        sqlite3_close(database_);
        database_ = nullptr;
    }
}

bool Database::initialize() {
    if (database_ == nullptr) return false;

    const char* sql = R"SQL(
        CREATE TABLE IF NOT EXISTS songs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE COLLATE NOCASE,
            filepath TEXT NOT NULL,
            filesize INTEGER NOT NULL DEFAULT 0,
            status TEXT NOT NULL DEFAULT 'NEW'
        );
    )SQL";

    char* errorMessage = nullptr;

    const int result = sqlite3_exec(database_,sql,nullptr,nullptr,&errorMessage);

    if (result != SQLITE_OK) {
        sqlite3_free(errorMessage);
        return false;
    }

    return true;
}

sqlite3* Database::handle() const {
    return database_;
}