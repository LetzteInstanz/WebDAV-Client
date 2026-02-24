#include "JsonObject.h"

JsonObject::JsonObject() : _parent(nullptr) {}

JsonObject::JsonObject(JsonObject* parent, std::string_view key) : _parent(parent), _key(key) {}

JsonObject::~JsonObject() = default;

nlohmann::json JsonObject::get_json_object(std::string_view key) const {
    const nlohmann::json json_object = get_json_object();
    const auto it = json_object.find(key);
    return it == std::end(json_object) ? nlohmann::json::object() : *it;
}

void JsonObject::set_json_object(std::string_view key, nlohmann::json&& object) { set_changed_value(key, std::move(object)); }

nlohmann::json JsonObject::get_json_object() const {
    assert(_parent);
    assert(_key);
    return _parent->get_json_object(*_key);
}

void JsonObject::set_json_object(nlohmann::json&& object) {
    assert(_parent);
    assert(_key);
    _parent->set_json_object(*_key, std::move(object));
}
