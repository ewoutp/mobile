package notification;

import android.R;
import android.content.Context;
import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import java.lang.Runnable;

public abstract class NotificationHelper {
    private static int lastId = 1;

    public static int notify(final Activity activity, final String title, final String text) {
        final int id = nextId();
        activity.runOnUiThread(new Runnable() {
            public void run() {
                NotificationManager nMgr = (NotificationManager) activity.getSystemService(Context.NOTIFICATION_SERVICE);
                Notification n = new Notification.Builder(activity)
                    .setContentTitle(title)
                    .setContentText(text)
                    .setSmallIcon(R.drawable.ic_media_play)
                    .setCategory(Notification.CATEGORY_SERVICE)
                    .setOngoing(true)
                    .build();
                nMgr.notify(id, n);
            }
        });
        return id;
    }

    public static void close(final Activity activity, final int id) {
        activity.runOnUiThread(new Runnable() {
            public void run() {
                NotificationManager nMgr = (NotificationManager) activity.getSystemService(Context.NOTIFICATION_SERVICE);
                nMgr.cancel(id);
            }
        });
    }

    private static int nextId() {
        synchronized (NotificationHelper.class) {
            return ++lastId;
        }
    }
}
