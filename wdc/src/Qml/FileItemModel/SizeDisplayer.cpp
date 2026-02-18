#include "SizeDisplayer.h"

QString SizeDisplayer::to_string(std::uint64_t bytes) {
    double sz = bytes;
    int i = 0;
    for (; sz >= 1024 && i < SizeDisplayer::size; ++i)
        sz /= 1024;

    assert(i < _prefixes.size());
    return QString("%1 %2").arg(_locale.toString(sz, 'g', 3), _prefixes[i]);
}

const std::array<QString, SizeDisplayer::size> SizeDisplayer::_prefixes{QObject::tr("B"), QObject::tr("K"), QObject::tr("M"), QObject::tr("G"), QObject::tr("T"), QObject::tr("P"), QObject::tr("E")};
const QLocale SizeDisplayer::_locale;
