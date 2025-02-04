#include "SettingsJsonFile.h"

#include "../Logger.h"
#include "../Qml/Sort/SortParam.h"
#include "../Qml/FileItemModel/Role.h"
#include "../Util.h"

namespace Qml {
    using Role = FileItemModelRole;
}

const std::unordered_map<std::string, Qml::SortParam> SettingsJsonFile::_supported_sort_params{
    {"type",              {Qml::Role::FileFlag,     QObject::tr("Type (directories are higher)"), false, Qml::SortParam::compare_file_flag}},
    {"name",              {Qml::Role::Name,         QObject::tr("Name"),                          false, Qml::SortParam::compare_qstring}},
    {"modification_time", {Qml::Role::ModTime,      QObject::tr("Modification time"),             false, Qml::SortParam::compare_time_t}},
    {"creation_time",     {Qml::Role::CreationTime, QObject::tr("Creation time"),                 false, Qml::SortParam::compare_time_t}},
    {"size",              {Qml::Role::Size,         QObject::tr("Size"),                          false, Qml::SortParam::compare_uint64_t}},
    {"extension",         {Qml::Role::Extension,    QObject::tr("Filename extension"),            false, Qml::SortParam::compare_extension}}
};

const std::vector<std::string> SettingsJsonFile::_default_sort_param_order{"type", "name", "modification_time", "creation_time", "size", "extension"};

std::unordered_map<Qml::Role, std::string> SettingsJsonFile::_sort_param_json_id_by_role_map;

SettingsJsonFile::SettingsJsonFile(std::shared_ptr<Logger> logger) : JsonFile("config.json"), _logger(std::move(logger)) {
    assert(_supported_sort_params.size() == _default_sort_param_order.size());

    auto& out_map = _sort_param_json_id_by_role_map;
    if (out_map.empty())
        std::transform(std::begin(_supported_sort_params), std::end(_supported_sort_params), std::inserter(out_map, std::end(out_map)), [](const auto& pair) { return std::make_pair(pair.second.role, pair.first); });

    auto all_is_ok = true;
    nlohmann::json object = get_root_obj();
    auto it = object.find(_dl_path_key);
    auto exists = it != std::end(object);
    auto ok = exists && it->is_string();
    all_is_ok &= ok;
    if (exists && !ok)
        json_value_type_warning(_dl_path_key, QObject::tr("a string"));

    _download_path = ok ? it->get<std::string>() : QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();
    if (!ok)
        object[_dl_path_key] = _download_path;

    it = object.find(_log_level_key);
    exists = it != std::end(object);
    ok = exists && it->is_number_unsigned();
    all_is_ok &= ok;
    if (exists && !ok)
        json_value_type_warning(_log_level_key, QObject::tr("a number"));

    _log_level = ok ? to_type<QtMsgType>(it->get<QtMsgType>()) : QtWarningMsg;
    _logger->set_max_level(_log_level);
    if (!ok)
        object[_log_level_key] = _log_level;

    _sort_params.reserve(_supported_sort_params.size());
    it = object.find(_sort_param_array_key);
    exists = it != std::end(object);
    ok = exists && it->is_array() && read_sort_params(it->get<nlohmann::json::array_t>());
    all_is_ok &= ok;
    if (!ok) {
        if (exists && !it->is_array())
            json_value_type_warning(_sort_param_array_key, QObject::tr("an array"));

        _sort_params = get_default_sort_params();
        object[_sort_param_array_key] = to_json_array(_sort_params);
    }

    it = object.find(_cs_key);
    exists = it != std::end(object);
    ok = exists && it->is_boolean();
    all_is_ok &= ok;
    if (exists && !ok)
        json_value_type_warning(_cs_key, QObject::tr("a boolean value"));

    _case_sensitive = ok ? it->get<bool>() : true;
    if (!ok)
        object[_cs_key] = _case_sensitive;

    if (!all_is_ok)
        set_root_obj(std::move(object));
}

std::string SettingsJsonFile::get_download_path() const { return _download_path; }

void SettingsJsonFile::set_download_path(std::string_view path) {
    if (_download_path == path)
        return;

    _download_path = path;
    set_value(_dl_path_key, std::as_const(_download_path));
}

QtMsgType SettingsJsonFile::get_max_log_level() const noexcept { return _log_level; }

void SettingsJsonFile::set_max_log_level(QtMsgType level) {
    if (_log_level == level)
        return;

    _log_level = level;
    _logger->set_max_level(_log_level);
    set_value(_log_level_key, std::remove_reference_t<QtMsgType>(_log_level));
}

std::vector<Qml::SortParam> SettingsJsonFile::get_sort_params() const { return _sort_params; }

void SettingsJsonFile::set_sort_params(const std::vector<Qml::SortParam>& params) {
    if (_sort_params == params)
        return;

    _sort_params = params;
    set_value(_sort_param_array_key, to_json_array(_sort_params));
    if (_sort_param_changed_signal)
        _sort_param_changed_signal();
}

bool SettingsJsonFile::get_search_cs_flag() const noexcept { return _case_sensitive; }

void SettingsJsonFile::set_search_cs_flag(bool case_sensitive) {
    if (_case_sensitive == case_sensitive)
        return;

    _case_sensitive = case_sensitive;
    set_value(_cs_key, std::remove_reference_t<bool>(_case_sensitive));
}

void SettingsJsonFile::set_notification_func(std::function<void ()>&& func) noexcept { _sort_param_changed_signal = std::move(func); }

SettingsJsonFile::SortParamVector SettingsJsonFile::get_default_sort_params() {
    SortParamVector result;
    result.reserve(_supported_sort_params.size());
    for (const std::string& id : _default_sort_param_order) {
        const auto it = _supported_sort_params.find(id);
        assert(it != std::end(_supported_sort_params));
        result.push_back(it->second);
    }
    return result;
}

bool SettingsJsonFile::read_sort_params(const nlohmann::json& array) {
    for (auto it = std::begin(array), end = std::end(array); it != end; ++it) {
        if (!it->is_object()) {
            qWarning().noquote() << QObject::tr("An element of the sort parameter array isn't a object");
            return false;
        }
        const nlohmann::json::object_t object = it->get<nlohmann::json::object_t>();
        std::map<std::string, nlohmann::json>::const_iterator value_it = object.find(_sort_param_id_key);
        if (value_it == std::end(object)) {
            json_value_exist_warning(_sort_param_id_key);
            return false;
        }
        if (!value_it->second.is_string()) {
            json_value_type_warning(_sort_param_id_key, QObject::tr("a string"));
            return false;
        }
        const auto json_id = value_it->second.get<std::string>();
        const auto param_it = _supported_sort_params.find(json_id);
        if (param_it == std::end(_supported_sort_params)) {
            qWarning(qUtf8Printable(QObject::tr("The sort param isn't supported: %s")), qUtf8Printable(json_id.c_str()));
            return false;
        }
        value_it = object.find(_sort_param_desc_key);
        if (value_it == std::end(object)) {
            json_value_exist_warning(_sort_param_desc_key);
            return false;
        }
        if (!value_it->second.is_boolean()) {
            json_value_type_warning(_sort_param_desc_key, QObject::tr("a boolean value"));
            return false;
        }
        Qml::SortParam param(param_it->second);
        param.descending = value_it->second.get<bool>();
        _sort_params.push_back(std::move(param));
    }
    const auto all = _sort_params.size() == _supported_sort_params.size();
    if (!all)
        qWarning() << QObject::tr("There aren't all sort parameters in the setting file");

    return all;
}

nlohmann::json SettingsJsonFile::to_json_array(const SortParamVector& params) {
    nlohmann::json array;
    for (const Qml::SortParam& param : params) {
        nlohmann::json obj(nlohmann::json::object());
        const auto it = _sort_param_json_id_by_role_map.find(param.role);
        assert(it != std::end(_sort_param_json_id_by_role_map));
        obj[_sort_param_id_key] = it->second;
        obj[_sort_param_desc_key] = param.descending;
        array.push_back(std::move(obj));
    }
    return array;
}

template<typename T>
void SettingsJsonFile::set_value(const char* key, T&& value) {
    nlohmann::json obj = get_root_obj();
    obj[key] = std::forward<T>(value);
    set_root_obj(std::move(obj));
}
