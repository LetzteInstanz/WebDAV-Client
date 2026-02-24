#include "RootSection.h"

#include "../Qml/Sort/SortParam.h"
#include "../Qml/FileItemModel/Role.h"

namespace JsonKeys {
    const constexpr char* log_level = "log_level";
    const constexpr char* case_sensitive = "case_sensitive";

    namespace Sorting {
        const constexpr char* array = "sort";
        const constexpr char* id = "id";
        const constexpr char* descending = "descending";
    }
}

namespace Qml { using Role = FileItemModelRole; }

namespace { std::unordered_map<Qml::Role, std::string> sort_param_json_id_by_role_map; }

const std::unordered_map<std::string, Qml::SortParam> supported_sort_params{
    {"type",              {Qml::Role::FileFlag,     QObject::tr("Type (directories are higher)"), false, Qml::SortParam::compare_file_flag}},
    {"name",              {Qml::Role::Name,         QObject::tr("Name"),                          false, Qml::SortParam::compare_qstring}},
    {"modification_time", {Qml::Role::ModTime,      QObject::tr("Modification time"),             false, Qml::SortParam::compare_sys_seconds}},
    {"creation_time",     {Qml::Role::CreationTime, QObject::tr("Creation time"),                 false, Qml::SortParam::compare_sys_seconds}},
    {"size",              {Qml::Role::Size,         QObject::tr("Size"),                          false, Qml::SortParam::compare_uint64_t}},
    {"extension",         {Qml::Role::Extension,    QObject::tr("Filename extension"),            false, Qml::SortParam::compare_extension}}
};

const std::array<std::string, 6> default_sort_param_order{"type", "name", "modification_time", "creation_time", "size", "extension"};

namespace Qml {
    void to_json(nlohmann::json& json, const SortParam& param) {
        const auto it = sort_param_json_id_by_role_map.find(param.role);
        assert(it != std::cend(sort_param_json_id_by_role_map));
        json = nlohmann::json{{JsonKeys::Sorting::id, it->second}, {JsonKeys::Sorting::descending, param.descending}};
    }

    void from_json(const nlohmann::json& json, SortParam& param) {
        constexpr const char* title = "SortParam";
        const std::string id = get_value_from_json<std::string>(json, title, JsonKeys::Sorting::id);
        const auto param_it = supported_sort_params.find(id);
        if (param_it == std::end(supported_sort_params)) {
            qWarning(qUtf8Printable(QObject::tr("%s: the sort parameter isn't supported: \"%s\"")), title, id.c_str());
            throw std::runtime_error("Unsupported sort parameter");
        }
        param = param_it->second;
        param.descending = get_value_from_json<bool>(json, title, JsonKeys::Sorting::descending);
    }
}

Settings::RootSection::RootSection(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object)
    : RootObject(std::move(get_root_object), std::move(set_root_object))
{
    assert(_get_root_object && _set_root_object);
    assert(default_sort_param_order.size() == supported_sort_params.size());
    {
        auto& out_map = sort_param_json_id_by_role_map;
        assert(out_map.empty());
        std::transform(std::begin(supported_sort_params), std::end(supported_sort_params), std::inserter(out_map, std::end(out_map)), [](const auto& pair) { return std::make_pair(pair.second.role, pair.first); });
    }
    nlohmann::json root = _get_root_object();
    auto needs_to_update = false;
    bool value_changed;
    _log_level = *get_value<QtMsgType>(root, []() { return QtWarningMsg; }, JsonKeys::log_level, value_changed);
    needs_to_update |= value_changed;
    const auto get_default_sort_params = []() {
        std::vector<Qml::SortParam> result;
        result.reserve(supported_sort_params.size());
        std::transform(std::begin(default_sort_param_order), std::end(default_sort_param_order), std::back_inserter(result), [](const std::string& id) { return supported_sort_params.find(id)->second; });
        return result;
    };
    _sort_params.reserve(supported_sort_params.size());
    try {
        _sort_params = *get_value<std::vector<Qml::SortParam>>(root, get_default_sort_params, JsonKeys::Sorting::array, value_changed);
        if (_sort_params.size() != supported_sort_params.size()) {
            qWarning(qUtf8Printable(QObject::tr("%s: the sort parameter array size and the default size aren't equal: %d")), "SortParam", _sort_params.size());
            throw std::runtime_error("The sort parameter array size and the default size aren't equal");
        }
    } catch (const std::runtime_error& /*e*/) {
        _sort_params = get_default_sort_params();
        root[JsonKeys::Sorting::array] = _sort_params;
        value_changed = true;
    }
    needs_to_update |= value_changed;

    _case_sensitive = *get_value<bool>(root, []() { return true; }, JsonKeys::case_sensitive, value_changed);
    needs_to_update |= value_changed;

    if (needs_to_update)
        _set_root_object(std::move(root));
}

Settings::RootSection::~RootSection() = default;

QtMsgType Settings::RootSection::get_log_level() const noexcept { return _log_level; }

bool Settings::RootSection::set_log_level(QtMsgType level) {
    if (_log_level == level)
        return false;

    _log_level = level;
    set_json_object(JsonKeys::log_level, std::as_const(_log_level));
    return true;
}

std::vector<Qml::SortParam> Settings::RootSection::get_sort_params() const { return _sort_params; }

bool Settings::RootSection::set_sort_params(const std::vector<Qml::SortParam>& params) {
    if (_sort_params == params)
        return false;

    _sort_params = params;
    set_json_object(JsonKeys::Sorting::array, std::as_const(_sort_params));
    return true;
}

bool Settings::RootSection::get_filter_cs_flag() const noexcept { return _case_sensitive; }

bool Settings::RootSection::set_filter_cs_flag(bool case_sensitive) {
    if (_case_sensitive == case_sensitive)
        return false;

    _case_sensitive = case_sensitive;
    set_json_object(JsonKeys::case_sensitive, std::as_const(_case_sensitive));
    return true;
}
