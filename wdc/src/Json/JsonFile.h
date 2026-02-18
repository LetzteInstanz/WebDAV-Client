#pragma once

class JsonFile {
public:
    JsonFile(std::string_view filename);
    virtual ~JsonFile();

    nlohmann::json get_root_object() const;
    void set_root_object(nlohmann::json&& json);

private:
    std::filesystem::path _path;
    std::fstream _file;
    std::optional<nlohmann::json> _json_data;
};
