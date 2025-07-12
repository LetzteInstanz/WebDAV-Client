#pragma once

namespace Qml {
    class IconProvider : public QQuickImageProvider {
        Q_OBJECT

    public:
        IconProvider();

        QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

    private:
        static const char* const _res_path;
    };
}
