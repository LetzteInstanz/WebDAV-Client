package org.qtproject.webdavclient;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class MyBroadcastReceiver extends BroadcastReceiver {
    public static native void sendNotificationId(int id);

    @Override public void onReceive(Context context, Intent intent) {
        int id = intent.getIntExtra("notification_id", -1);
        Log.d("MyBroadcastReceiver", "onReceive() is called: notification_id = " + id);
        sendNotificationId(id);
    }
}
