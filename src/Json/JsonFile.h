#pragma once

#include <fstream>
#include <string>
#include <string_view>
#include <utility>

#include <nlohmann/json.hpp>

class JsonFile {
public:
    JsonFile(std::string_view filename);
    virtual ~JsonFile();

protected:
    nlohmann::json get_root_obj() const { return _json_data; }
    template <typename T>
    void set_value(const char* key, T&& value);

private:
    void set_root_obj(nlohmann::json&& json);

private:
    bool _data_was_changed = false;
    std::string _path_to_file;
    std::fstream _file;
    nlohmann::json _json_data;
};

template<typename T>
void JsonFile::set_value(const char* key, T&& value) {
    nlohmann::json obj = get_root_obj();
    obj[key] = std::forward<T>(value);
    set_root_obj(std::move(obj));
}
