#pragma once

#include "../include/json/json.hpp"
#include <string>

class Json : public nlohmann::json {
    public:
        using nlohmann::json::json;
    
        Json(const nlohmann::json& other);
        Json(nlohmann::json&& other);
        
        static std::string stringify(const nlohmann::json& obj);
        static Json parse(const std::string& str);
};