#include "Settings.h"

#include "../Settings/Settings.h"

Qml::Settings::Settings(std::shared_ptr<::Settings> settings, QObject* parent) : QObject(parent), _settings(std::move(settings)) {
    for (std::size_t i = 0, sz = _desc_level_pairs.size(); i < sz; ++i)
        _indexByLogLevel.emplace(_desc_level_pairs[i].second, i);
}

Qml::Settings::~Settings() = default;

QString Qml::Settings::removeScheme(const QString& uri) {
#ifdef ANDROID
    const auto prefix = QString("content:/");
#else
    const auto prefix = QString("file://");
#endif
    const auto borderIt = std::begin(uri) + prefix.length();
    if (QStringView(std::begin(uri), borderIt) != prefix) {
        qCritical(qUtf8Printable(QObject::tr("Invalid URI: %s")), qUtf8Printable(uri));
        return QString();
    }
    return QStringView(borderIt, std::end(uri)).toString();
}

QString Qml::Settings::addScheme(const QString& uri) {
#ifdef ANDROID
    const auto prefix = QString("content:/%1");
#else
    const auto prefix = QString("file://%1");
#endif
    return prefix.arg(uri);
}

bool Qml::Settings::getAskPathFlag() const noexcept { return _settings->get_ask_path_flag(); }

void Qml::Settings::setAskPathFlag(bool ask) const { return _settings->set_ask_path_flag(ask); }

QString Qml::Settings::getDownloadPath() const { return QString::fromStdString(_settings->get_download_path()); }

void Qml::Settings::setDownloadPath(const QString& path) { _settings->set_download_path(path.toStdString()); }

int Qml::Settings::getCurrentLogLevel() const {
    const auto it = _indexByLogLevel.find(_settings->get_max_log_level());
    assert(it != std::end(_indexByLogLevel));
    return it->second;
}

void Qml::Settings::setCurrentLogLevel(int index) {
    const std::pair<QString, QtMsgType>& pair = _desc_level_pairs[index];
    _settings->set_max_log_level(pair.second);
}

bool Qml::Settings::getSearchCSFlag() const noexcept { return _settings->get_search_cs_flag(); }

void Qml::Settings::setSearchCSFlag(bool caseSensitive) { _settings->set_search_cs_flag(caseSensitive); }

QStringList Qml::Settings::getLevelDescList() const {
    QStringList dataList;
    std::transform(std::begin(_desc_level_pairs), std::end(_desc_level_pairs), std::back_inserter(dataList), [](const std::pair<QString, QtMsgType>& pair) { return pair.first; });
    return dataList;
}
