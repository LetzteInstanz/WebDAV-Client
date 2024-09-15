#include "Util.h"

#include "pch.h"

void json_value_exist_warning(const char* key) { qWarning(qUtf8Printable(QObject::tr("The value of the key \"%s\" doesn't exist")), key); }

void json_value_type_warning(const char* key, const QString& value_type) { qWarning(qUtf8Printable(QObject::tr("The value of the key \"%s\" isn't %s")), key, qUtf8Printable(value_type)); }
