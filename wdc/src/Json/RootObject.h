#pragma once

#include "JsonObject.h"

template<typename ReturnType>
static auto get_value_from_json(const nlohmann::json& json, const char* log_title, const char* key) {
    try {
        return json.at(key).get<ReturnType>();
    } catch (const nlohmann::json::exception& e) {
        qWarning(qUtf8Printable(QObject::tr("%s: %s: %s")), log_title, key, e.what());
        throw e;
    }
};

class RootObject : public JsonObject {
public:
    RootObject(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object);
    ~RootObject() override;

    nlohmann::json get_json_object(std::string_view key) const override;
    void set_json_object(std::string_view key, nlohmann::json&& object) override;

protected:
    const std::function<nlohmann::json ()> _get_root_object;
    const std::function<void (nlohmann::json&&)> _set_root_object;
};
