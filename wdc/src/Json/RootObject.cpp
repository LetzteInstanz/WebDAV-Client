#include "RootObject.h"

RootObject::RootObject(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object)
    : _get_root_object(std::move(get_root_object)), _set_root_object(std::move(set_root_object))
{
    assert(_get_root_object && _set_root_object);
}

RootObject::~RootObject() = default;

nlohmann::json RootObject::get_json_object(std::string_view key) const {
    const nlohmann::json root = _get_root_object();
    const auto it = root.find(key);
    return it == std::end(root) ? nlohmann::json::object() : *it;
}

void RootObject::set_json_object(std::string_view key, nlohmann::json&& object) {
    nlohmann::json root = _get_root_object();
    assert(root.find(key) == std::cend(root) || *root.find(key) != object);
    root[key] = std::move(object);
    _set_root_object(std::move(root));
}
