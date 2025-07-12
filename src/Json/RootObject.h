#pragma once

#include "JsonObject.h"

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
