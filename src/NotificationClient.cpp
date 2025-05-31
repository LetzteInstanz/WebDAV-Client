#include "NotificationClient.h"

namespace {
    void recieve_notification_id(JNIEnv* /*env*/, jobject /*thiz*/, jint notification_id) { qDebug().noquote() << QObject::tr("Notification ID = ") << notification_id; }
}

NotificationClient::NotificationClient(const QString& channel_name) {
    if (QNativeInterface::QAndroidApplication::sdkVersion() >= __ANDROID_API_T__) {
        const QFuture<QtAndroidPrivate::PermissionResult> requestResult = QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS");
        if (requestResult.result() != QtAndroidPrivate::Authorized)
            throw std::runtime_error("Failed to acquire permission to post notifications (required for Android 13+)");
    }
    const QJniObject j_channel_name = QJniObject::fromString(channel_name);
    if (!QJniObject::callStaticMethod<bool>("org/qtproject/webdavclient/NotificationClient", "init", QNativeInterface::QAndroidApplication::context(), j_channel_name.object<jstring>()))
        throw std::runtime_error("Failed to initialize notification client");

    const JNINativeMethod methods[] = {{"sendNotificationId", "(I)V", reinterpret_cast<void*>(recieve_notification_id)}};
    QJniEnvironment env;
    if (!env.registerNativeMethods("org/qtproject/webdavclient/MyBroadcastReceiver", methods, 1))
        throw std::runtime_error("Failed to register the JNI method");
}

void NotificationClient::create_or_update_notification(std::int32_t notification_id, const QString& title, const QString& text, int progress_value, int max_progress_value, const QString& button_text) {
    const jint j_notification_id = notification_id;
    const QJniObject j_title = QJniObject::fromString(title);
    const QJniObject j_text = QJniObject::fromString(text);
    const jint j_progress_value = progress_value;
    const jint j_max_progress_value = max_progress_value;
    const QJniObject j_button_text = QJniObject::fromString(button_text);
    QJniObject::callStaticMethod<void>("org/qtproject/webdavclient/NotificationClient", "createOrUpdateNotification",
                                       QNativeInterface::QAndroidApplication::context(), j_notification_id, j_title.object<jstring>(), j_text.object<jstring>(), j_progress_value, j_max_progress_value, j_button_text.object<jstring>());
}

void NotificationClient::hide_notification(std::int32_t notification_id) { QJniObject::callStaticMethod<void>("org/qtproject/webdavclient/NotificationClient", "hideNotification", jint(notification_id)); }

void NotificationClient::hide_all_notifications() { QJniObject::callStaticMethod<void>("org/qtproject/webdavclient/NotificationClient", "hideAllNotifications"); }
