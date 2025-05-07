#include "SettingsJsonFile.h"

#include "../Logger.h"
#include "../Qml/Sort/SortParam.h"
#include "../Qml/FileItemModel/Role.h"
#include "../Util.h"

namespace JsonKeys {
    const constexpr char* download_path = "download_path";
    const constexpr char* log_level = "log_level";

    namespace Sorting {
        const constexpr char* array = "sort";
        const constexpr char* id = "id";
        const constexpr char* descending = "descending";
        const constexpr char* case_sensitive = "case_sensitive";
    }
}

namespace Qml {
    using Role = FileItemModelRole;
}

std::unordered_map<Qml::Role, std::string> sort_param_json_id_by_role_map;

const std::unordered_map<std::string, Qml::SortParam> supported_sort_params{
    {"type",              {Qml::Role::FileFlag,     QObject::tr("Type (directories are higher)"), false, Qml::SortParam::compare_file_flag}},
    {"name",              {Qml::Role::Name,         QObject::tr("Name"),                          false, Qml::SortParam::compare_qstring}},
    {"modification_time", {Qml::Role::ModTime,      QObject::tr("Modification time"),             false, Qml::SortParam::compare_sys_seconds}},
    {"creation_time",     {Qml::Role::CreationTime, QObject::tr("Creation time"),                 false, Qml::SortParam::compare_sys_seconds}},
    {"size",              {Qml::Role::Size,         QObject::tr("Size"),                          false, Qml::SortParam::compare_uint64_t}},
    {"extension",         {Qml::Role::Extension,    QObject::tr("Filename extension"),            false, Qml::SortParam::compare_extension}}
};

namespace Qml {
    void to_json(nlohmann::json& json, const SortParam& param) {
        const auto it = sort_param_json_id_by_role_map.find(param.role);
        assert(it != std::cend(sort_param_json_id_by_role_map));
        json = nlohmann::json{{JsonKeys::Sorting::id, it->second}, {JsonKeys::Sorting::descending, param.descending}};
    }

    void from_json(const nlohmann::json& json, SortParam& param) {
        const std::string id = get_value_from_json<std::string>(json, JsonKeys::Sorting::id);
        const auto param_it = supported_sort_params.find(id);
        if (param_it == std::cend(supported_sort_params)) {
            qWarning(qUtf8Printable(QObject::tr("The sort param isn't supported: \"%s\"")), qUtf8Printable(id.c_str()));
            throw std::runtime_error("Unsupported sort param");
        }
        param = param_it->second;
        param.descending = get_value_from_json<bool>(json, JsonKeys::Sorting::descending);
    }
}

const std::array<std::string, 6> default_sort_param_order{"type", "name", "modification_time", "creation_time", "size", "extension"};

SettingsJsonFile::SettingsJsonFile(std::shared_ptr<Logger> logger) : JsonFile("config.json"), _logger(std::move(logger)) {
    assert(supported_sort_params.size() == default_sort_param_order.size());
    auto& out_map = sort_param_json_id_by_role_map;
    if (out_map.empty())
        std::transform(std::begin(supported_sort_params), std::end(supported_sort_params), std::inserter(out_map, std::end(out_map)), [](const auto& pair) { return std::make_pair(pair.second.role, pair.first); });

    const nlohmann::json object = get_root_obj();
    const auto get_setting = [this]<typename ReturnType>(const nlohmann::json& object, const std::function<ReturnType ()>& get_default_value_func, const char* key) {
        try {
            return object.at(key).get<ReturnType>();
        } catch (const nlohmann::json::exception& e) {
            qWarning(qUtf8Printable(QObject::tr("config.json: %s: %s")), key, e.what());
            const auto default_value = get_default_value_func();
            set_value(key, std::as_const(default_value));
            return default_value;
        }
        catch (const std::runtime_error& /*e*/) {
            const auto default_value = get_default_value_func();
            set_value(key, std::as_const(default_value));
            return default_value;
        }
    };
    _download_path = get_setting.template operator()<std::string>(object, [](){ return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString(); }, JsonKeys::download_path);
    _log_level = get_setting.template operator()<QtMsgType>(object, [](){ return QtWarningMsg; }, JsonKeys::log_level);
    _logger->set_max_level(_log_level);

    const auto get_default_sort_params = []() {
        SortParamVector result;
        result.reserve(supported_sort_params.size());
        for (const std::string& id : default_sort_param_order) {
            const auto it = supported_sort_params.find(id);
            assert(it != std::end(supported_sort_params));
            result.push_back(it->second);
        }
        return result;
    };
    _sort_params.reserve(supported_sort_params.size());
    _sort_params = get_setting.template operator()<SortParamVector>(object, get_default_sort_params, JsonKeys::Sorting::array);
    _case_sensitive = get_setting.template operator()<bool>(object, [](){ return true; }, JsonKeys::Sorting::case_sensitive);
}

std::string SettingsJsonFile::get_download_path() const { return _download_path; }

void SettingsJsonFile::set_download_path(std::string_view path) {
    if (_download_path == path)
        return;

    _download_path = path;
    set_value(JsonKeys::download_path, std::as_const(_download_path));
}

QtMsgType SettingsJsonFile::get_max_log_level() const noexcept { return _log_level; }

void SettingsJsonFile::set_max_log_level(QtMsgType level) {
    if (_log_level == level)
        return;

    _log_level = level;
    _logger->set_max_level(_log_level);
    set_value(JsonKeys::log_level, std::remove_reference_t<QtMsgType>(_log_level));
}

std::vector<Qml::SortParam> SettingsJsonFile::get_sort_params() const { return _sort_params; }

void SettingsJsonFile::set_sort_params(const std::vector<Qml::SortParam>& params) {
    if (_sort_params == params)
        return;

    _sort_params = params;
    set_value(JsonKeys::Sorting::array, std::as_const(_sort_params));
    if (_sort_param_changed_signal)
        _sort_param_changed_signal();
}

bool SettingsJsonFile::get_search_cs_flag() const noexcept { return _case_sensitive; }

void SettingsJsonFile::set_search_cs_flag(bool case_sensitive) {
    if (_case_sensitive == case_sensitive)
        return;

    _case_sensitive = case_sensitive;
    set_value(JsonKeys::Sorting::case_sensitive, std::remove_reference_t<bool>(_case_sensitive));
}

void SettingsJsonFile::set_notification_func(std::function<void ()>&& func) noexcept { _sort_param_changed_signal = std::move(func); }
