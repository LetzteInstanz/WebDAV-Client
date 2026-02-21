#include "FileSystemInfo.h"

#include "FileItemModel/SizeDisplayer.h"

Qml::FileSystemInfo::~FileSystemInfo() { qDebug().noquote().nospace() << QObject::tr("Qml::FileSystemInfo: destroyed"); }

QString Qml::FileSystemInfo::getSizeStr() const { return SizeDisplayer::to_string(size); }
