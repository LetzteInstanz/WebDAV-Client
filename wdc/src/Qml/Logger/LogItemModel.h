#pragma once

#include "../../Logger.h"

namespace Qml {
    class LogItemModel : public QAbstractTableModel {
        Q_OBJECT

    public:
        explicit LogItemModel(std::shared_ptr<Logger> logger, QObject* parent = nullptr);
        ~LogItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void copyToClipboard(int index) const;
        Q_INVOKABLE void copyAllToClipboard() const;

    private:
        void update();

    private:
        using Message = std::tuple<QColor, QDateTime, QString>;
        enum class Role {Colour = Qt::UserRole, TimeStr, Text, EnumSize};

        std::shared_ptr<Logger> _logger;
        const QString _time_format;
        std::atomic<bool> _needs_to_update = false;
        std::mutex _mutex;
        QTimer _update_timer;
        std::deque<Logger::Message> _sync_log;
        std::deque<Message> _log;
    };
}
