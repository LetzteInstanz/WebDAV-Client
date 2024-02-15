#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtLogging>

class SettingsJsonFile;

namespace Qml {
    class Settings : public QObject {
        Q_OBJECT

    public:
        explicit Settings(std::shared_ptr<SettingsJsonFile> settings, QObject* parent = nullptr);
        ~Settings() override;

        Q_INVOKABLE QString getDownloadPath() const;
        Q_INVOKABLE void setDownloadPath(const QString& path);
        Q_INVOKABLE int getCurrentLogLevel() const;
        Q_INVOKABLE void setCurrentLogLevel(int index);
        Q_INVOKABLE bool getSearchCSFlag() const;
        Q_INVOKABLE void setSearchCSFlag(bool caseSensitive);
        QStringList get_level_desc_list() const;

    private:
        std::shared_ptr<SettingsJsonFile> _settings;
        const std::vector<std::pair<QString, QtMsgType>> _desc_level_pairs{{tr("Debug"), QtDebugMsg}, {tr("Information"), QtInfoMsg},
                                                                           {tr("Warning"), QtWarningMsg}, {tr("Critical"), QtCriticalMsg},
                                                                           {tr("Fatal"), QtFatalMsg}};
        std::unordered_map<QtMsgType, int> _indexByLogLevel;
    };
}
