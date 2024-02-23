#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>

#include <QAbstractListModel>
#include <QByteArray>
#include <QColor>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <Qt>

class Logger;

namespace Qml {
    class LogItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit LogItemModel(std::shared_ptr<Logger> logger, QObject* parent = nullptr);
        ~LogItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void copyToClipboard(int index);
        Q_INVOKABLE void copyAllToClipboard();

    private:
        void update();

    private:
        using Message = std::pair<QColor, QString>;
        enum class Role {Colour = Qt::UserRole, Text, EnumSize};

        std::shared_ptr<Logger> _logger;
        std::atomic<bool> _needs_to_update = false;
        std::mutex _mutex;
        QTimer _update_timer;
        std::deque<Message> _sync_log;
        std::deque<Message> _log;
    };
}
