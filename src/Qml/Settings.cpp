#include "Settings.h"

#include "../Json/SettingsJsonFile.h"

using namespace Qml;

Settings::Settings(std::shared_ptr<SettingsJsonFile> settings, QObject* parent) : QObject(parent), _settings(std::move(settings)) {
    for (std::size_t i = 0, sz = _desc_level_pairs.size(); i < sz; ++i)
        _indexByLogLevel.emplace(_desc_level_pairs[i].second, i);
}

Settings::~Settings() = default;

QString Settings::getDownloadPath() const { return QString::fromStdString(_settings->get_download_path()); }

void Settings::setDownloadPath(const QString& path) { _settings->set_download_path(path.toStdString()); }

int Settings::getCurrentLogLevel() const {
    const auto it = _indexByLogLevel.find(_settings->get_max_log_level());
    return it->second;
}

void Settings::setCurrentLogLevel(int index) {
    const std::pair<QString, QtMsgType>& pair = _desc_level_pairs[index];
    _settings->set_max_log_level(pair.second);
}

bool Settings::getSearchCSFlag() const { return _settings->get_search_cs_flag(); }

void Settings::setSearchCSFlag(bool caseSensitive) { _settings->set_search_cs_flag(caseSensitive); }

QStringList Settings::getLevelDescList() const {
    QStringList dataList;
    std::transform(std::begin(_desc_level_pairs), std::end(_desc_level_pairs), std::back_inserter(dataList), [](const std::pair<QString, QtMsgType>& pair) { return pair.first; });
    return dataList;
}
