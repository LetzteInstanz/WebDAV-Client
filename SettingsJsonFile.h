#pragma once

#include <QJsonObject>
#include <QString>

#include "JsonFile.h"

class SettingsJsonFile final : public JsonFile {
public:
    SettingsJsonFile();

    QString get_download_path() const;
    void set_download_path(const QString& path);

private:
    void set_value(QJsonObject&& obj, const QString& value);

private:
    static const char* const _dl_path;
};
