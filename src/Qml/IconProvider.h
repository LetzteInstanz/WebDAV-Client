#pragma once

#include <unordered_map>

#include <QObject>
#include <QPixmap>
#include <QQuickImageProvider>
#include <QSize>
#include <QString>

namespace Qml {
    class IconProvider : public QQuickImageProvider {
        Q_OBJECT

    public:
        IconProvider();

        QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

    private:
        static const char* const _res_path;
        static const std::unordered_map<QString, QString> _icon_name_by_extension_map;
    };
}
