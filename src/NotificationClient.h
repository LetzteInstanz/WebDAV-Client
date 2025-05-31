#pragma once

class NotificationClient {
public:
    NotificationClient(const QString& channel_name);

    void create_or_update_notification(std::int32_t notification_id, const QString& title, const QString& text, int progress_value, int max_progress_value, const QString& button_text);
    void hide_notification(std::int32_t notification_id);
    void hide_all_notifications();
};
