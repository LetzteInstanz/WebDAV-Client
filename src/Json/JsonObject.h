#pragma once

class JsonObject {
public:
    JsonObject();
    JsonObject(JsonObject* parent, std::string_view key);
    virtual ~JsonObject();

    virtual nlohmann::json get_json_object(std::string_view key) const;
    virtual void set_json_object(std::string_view key, nlohmann::json&& object);

protected:
    nlohmann::json get_json_object() const;
    void set_json_object(nlohmann::json&& object);

    template <typename ReturnType>
    static std::optional<ReturnType> get_value(nlohmann::json& object, const std::function<ReturnType ()>& get_default_value_func, const char* key, bool& value_changed);

    template<typename Type>
    void set_value(std::string_view key, Type&& value);

private:
    JsonObject* const _parent;
    const std::optional<std::string> _key;
};

template <typename ReturnType>
std::optional<ReturnType> JsonObject::get_value(nlohmann::json& object, const std::function<ReturnType ()>& get_default_value_func, const char* key, bool& value_changed) {
    value_changed = false;
    try {
        return object.at(key).get<ReturnType>();
    }
    catch (const nlohmann::json::exception& e) {
        qWarning(qUtf8Printable(QObject::tr("JsonObject::get_value(): %s: %s")), key, e.what());
        if (!get_default_value_func)
            return {};

        const auto default_value = get_default_value_func();
        object[key] = default_value;
        value_changed = true;
        return default_value;
    }
}

template<typename Type>
void JsonObject::set_value(std::string_view key, Type&& value) {
    assert(_parent);
    nlohmann::json json_object = get_json_object();
    assert(json_object.find(key) == std::cend(json_object) || *json_object.find(key) != value);
    json_object[key] = std::forward<Type>(value);
    set_json_object(std::move(json_object));
}
