#include "Json.hpp"
#include <iostream>

Json::Json(const nlohmann::json& other): nlohmann::json(other) {}
Json::Json(nlohmann::json&& other): nlohmann::json(std::move(other)) {}

std::string Json::stringify(const nlohmann::json& obj) {
    return obj.dump();
}

Json Json::parse(const std::string& str) {
    return nlohmann::json::parse(str);
}