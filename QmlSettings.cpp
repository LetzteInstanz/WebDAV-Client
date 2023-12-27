#include "QmlSettings.h"

#include "SettingsJsonFile.h"

QmlSettings::QmlSettings(std::shared_ptr<SettingsJsonFile> settings, QObject* parent) : QObject(parent), _settings(settings) {
    for (size_t i = 0, sz = _desc_level_pairs.size(); i < sz; ++i)
        _indexByLogLevel.emplace(_desc_level_pairs[i].second, i);
}

QString QmlSettings::getDownloadPath() const { return _settings->get_download_path(); }

void QmlSettings::setDownloadPath(const QString& path) { _settings->set_download_path(path); }

int QmlSettings::getCurrentLogLevel() const {
    const auto it = _indexByLogLevel.find(_settings->get_max_log_level());
    return it->second;
}

void QmlSettings::setCurrentLogLevel(const int index) {
    const std::pair<QString, QtMsgType>& pair = _desc_level_pairs[index];
    _settings->set_max_log_level(pair.second);
}

QStringList QmlSettings::get_level_desc_list() const {
    QStringList dataList;
    std::transform(std::begin(_desc_level_pairs), std::end(_desc_level_pairs), std::back_inserter(dataList), [](const std::pair<QString, QtMsgType>& pair) { return pair.first; });
    return dataList;
}
