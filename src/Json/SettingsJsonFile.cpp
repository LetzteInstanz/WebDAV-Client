#include "SettingsJsonFile.h"

#include "../Logger.h"
#include "../Qml/Sort/SortParam.h"
#include "../Qml/FileItemModel/Role.h"
#include "../Util.h"

namespace Qml {
    using Role = FileItemModelRole;
}

const char* const SettingsJsonFile::_dl_path_key = "download_path";
const char* const SettingsJsonFile::_log_level_key = "log_level";
const char* const SettingsJsonFile::_sort_param_array_key = "sort";
const char* const SettingsJsonFile::_sort_param_id_key = "id";
const char* const SettingsJsonFile::_sort_param_desc_key = "descending";

const std::unordered_map<QString, Qml::SortParam> SettingsJsonFile::_supported_sort_params{
    {QStringLiteral("type"),              {Qml::Role::FileFlag,  QObject::tr("Type (directories are higher)"), false, Qml::SortParam::compare_file_flag}},
    {QStringLiteral("name"),              {Qml::Role::Name,      QObject::tr("Name"),                          false, Qml::SortParam::compare_qstring}},
    {QStringLiteral("modification_time"), {Qml::Role::ModTime,   QObject::tr("Modification time"),             false, Qml::SortParam::compare_time_t}},
    {QStringLiteral("extension"),         {Qml::Role::Extension, QObject::tr("Filename extension"),            false, Qml::SortParam::compare_extension}}
};

const std::vector<QString> SettingsJsonFile::_default_sort_param_order{QStringLiteral("type"), QStringLiteral("name"), QStringLiteral("modification_time"), QStringLiteral("extension")};

std::unordered_map<Qml::Role, QString> SettingsJsonFile::_sort_param_json_id_by_role_map;

SettingsJsonFile::SettingsJsonFile(std::shared_ptr<Logger> logger) : JsonFile("config.json"), _logger(std::move(logger)) {
    auto& out_map = _sort_param_json_id_by_role_map;
    if (out_map.empty())
        std::transform(std::begin(_supported_sort_params), std::end(_supported_sort_params), std::inserter(out_map, std::end(out_map)), [](const auto& pair) { return std::make_pair(pair.second.role, pair.first); });

    auto all_is_ok = true;
    QJsonObject obj = get_root_obj();
    auto it = obj.find(_dl_path_key);
    auto exists = it != std::end(obj);
    auto ok = exists && it->isString();
    all_is_ok &= ok;
    if (exists && !ok)
        json_value_type_warning(_dl_path_key, QObject::tr("a string"));

    _download_path = ok ? it->toString() : QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (!ok)
        obj[_dl_path_key] = _download_path;

    it = obj.find(_log_level_key);
    exists = it != std::end(obj);
    ok = exists && it->isDouble();
    all_is_ok &= ok;
    if (exists && !ok)
        json_value_type_warning(_log_level_key, QObject::tr("a number"));

    _log_level = ok ? static_cast<QtMsgType>(it->toInt()) : QtWarningMsg;
    _logger->set_max_level(_log_level);
    if (!ok)
        obj[_log_level_key] = _log_level;

    _sort_params.reserve(_supported_sort_params.size());
    it = obj.find(_sort_param_array_key);
    exists = it != std::end(obj);
    ok = exists && it->isArray() && read_sort_params(it->toArray());
    all_is_ok &= ok;
    if (!ok) {
        if (exists && !it->isArray())
            json_value_type_warning(_sort_param_array_key, QObject::tr("an array"));

        _sort_params = get_default_sort_params();
        obj[_sort_param_array_key] = to_json_array(_sort_params);
    }
    if (!all_is_ok)
        set_root_obj(std::move(obj));
}

QString SettingsJsonFile::get_download_path() const { return _download_path; }

void SettingsJsonFile::set_download_path(const QStringView& path) {
    if (_download_path == path)
        return;

    _download_path = path.toString();
    set_value(_dl_path_key, std::as_const(_download_path));
}

QtMsgType SettingsJsonFile::get_max_log_level() const { return _log_level; }

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
}

SettingsJsonFile::SortParamVector SettingsJsonFile::get_default_sort_params() {
    SortParamVector result;
    result.reserve(_supported_sort_params.size());
    for (const QString& id : _default_sort_param_order) {
        const auto it = _supported_sort_params.find(id);
        assert(it != std::end(_supported_sort_params));
        result.push_back(it->second);
    }
    return result;
}

bool SettingsJsonFile::read_sort_params(const QJsonArray& array) {
    for (auto it = std::begin(array), end = std::end(array); it != end; ++it) {
        const QJsonValue& val = *it;
        if (!val.isObject()) {
            qWarning().noquote() << QObject::tr("An element of the sort parameter array isn't a object");
            return false;
        }
        const QJsonObject obj = val.toObject();
        auto val_it = obj.find(_sort_param_id_key);
        if (val_it == std::end(obj)) {
            json_value_exist_warning(_sort_param_id_key);
            return false;
        }
        if (!val_it->isString()) {
            json_value_type_warning(_sort_param_id_key, QObject::tr("a string"));
            return false;
        }
        const QString json_id = val_it->toString();
        const auto param_it = _supported_sort_params.find(json_id);
        if (param_it == std::end(_supported_sort_params)) {
            qWarning(qUtf8Printable(QObject::tr("The sort param isn't supported: %s")), qUtf8Printable(json_id));
            return false;
        }
        val_it = obj.find(_sort_param_desc_key);
        if (val_it == std::end(obj)) {
            json_value_exist_warning(_sort_param_desc_key);
            return false;
        }
        if (!val_it->isBool()) {
            json_value_type_warning(_sort_param_desc_key, QObject::tr("a boolean value"));
            return false;
        }
        Qml::SortParam param(param_it->second);
        param.descending = val_it->toBool();
        _sort_params.push_back(std::move(param));
    }
    return true;
}

QJsonArray SettingsJsonFile::to_json_array(const SortParamVector& params) {
    QJsonArray array;
    for (const Qml::SortParam& param : params) {
        QJsonObject obj;
        const auto it = _sort_param_json_id_by_role_map.find(param.role);
        assert(it != std::end(_sort_param_json_id_by_role_map));
        obj[_sort_param_id_key] = it->second;
        obj[_sort_param_desc_key] = param.descending;
        array.append(std::move(obj));
    }
    return array;
}

template<typename T>
void SettingsJsonFile::set_value(const char* key, T&& value) {
    QJsonObject obj = get_root_obj();
    obj[key] = std::forward<T>(value);
    set_root_obj(std::move(obj));
}
