#include "IconProvider.h"

#include "../pch.h"

using namespace Qml;

const char* const IconProvider::_res_path = ":/res/icons/";

IconProvider::IconProvider() : QQuickImageProvider(QQmlImageProviderBase::Pixmap) {}

QPixmap IconProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) {
    const QPixmap pixmap(_res_path + id);
    if (size)
        *size = QSize(pixmap.height(), pixmap.width());

    return pixmap;
}
