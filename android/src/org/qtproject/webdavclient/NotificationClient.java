package org.qtproject.webdavclient;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.BitmapFactory;
import android.util.Log;

public class NotificationClient {
    public static boolean init(Context context, String channelName) {
        try {
            Log.d("NotificationClient", "Initializing…");
            m_notificationManager = (NotificationManager)context.getSystemService(Context.NOTIFICATION_SERVICE);
            NotificationChannel notificationChannel = new NotificationChannel("WebDAV Client", channelName, NotificationManager.IMPORTANCE_DEFAULT);
            m_notificationManager.createNotificationChannel(notificationChannel);
            m_builder = new Notification.Builder(context, notificationChannel.getId());
            Bitmap icon = BitmapFactory.decodeResource(context.getResources(), R.drawable.icon);
            m_builder.setSmallIcon(R.drawable.icon).setLargeIcon(icon).setOngoing(true);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    public static void createOrUpdateNotification(Context context, int notificationId, String title, String text, int progressValue, int maxProgressValue, String buttonText) {
        try {
            Log.d("NotificationClient", "createOrUpdateNotification() is called: notificationId = " + notificationId + ", title = " + title + ", text = " + text + ", progressValue = " + progressValue + ", maxProgressValue = " + maxProgressValue + ", buttonText = " + buttonText);
            Intent actionIntent = new Intent(context, MyBroadcastReceiver.class);
            actionIntent.putExtra("notification_id", notificationId);
            PendingIntent pendingIntent = PendingIntent.getBroadcast(context, notificationId, actionIntent, PendingIntent.FLAG_IMMUTABLE | PendingIntent.FLAG_UPDATE_CURRENT);
            Notification.Action action = new Notification.Action.Builder(0, buttonText, pendingIntent).build();
            m_builder.setContentTitle(title).setContentText(text).setActions(action).setProgress(maxProgressValue, progressValue, false);
            m_notificationManager.notify(notificationId, m_builder.build());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void hideNotification(int notificationId) {
        Log.d("NotificationClient", "hideNotification() is called: notificationId = " + notificationId);
        m_notificationManager.cancel(notificationId);
    }

    public static void hideAllNotifications() {
        Log.d("NotificationClient", "hideAllNotifications() is called");
        m_notificationManager.cancelAll();
    }

    private static NotificationManager m_notificationManager;
    private static Notification.Builder m_builder;
}
