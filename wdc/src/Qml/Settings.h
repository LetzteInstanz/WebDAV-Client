#pragma once

class Settings;

namespace Qml {
    class Settings : public QObject {
        Q_OBJECT

    public:
        explicit Settings(std::shared_ptr<::Settings> settings, QObject* parent = nullptr);
        ~Settings() override;

        Q_INVOKABLE static QString removeScheme(const QString& uri);
        Q_INVOKABLE static QString addScheme(const QString& uri);
        Q_INVOKABLE bool getAskPathFlag() const noexcept;
        Q_INVOKABLE void setAskPathFlag(bool ask) const;
        Q_INVOKABLE QString getDownloadPath() const;
        Q_INVOKABLE void setDownloadPath(const QString& path);
        Q_INVOKABLE int getCurrentLogLevel() const;
        Q_INVOKABLE void setCurrentLogLevel(int index);
        Q_INVOKABLE bool getFilterCSFlag() const noexcept;
        Q_INVOKABLE void setFilterCSFlag(bool caseSensitive);
        Q_INVOKABLE QStringList getLevelDescList() const;

    private:
        std::shared_ptr<::Settings> _settings;
        const std::vector<std::pair<QString, QtMsgType>> _desc_level_pairs{{tr("Debug"), QtDebugMsg}, {tr("Information"), QtInfoMsg},
                                                                           {tr("Warning"), QtWarningMsg}, {tr("Critical"), QtCriticalMsg},
                                                                           {tr("Fatal"), QtFatalMsg}};
        std::unordered_map<QtMsgType, int> _indexByLogLevel;
    };
}
