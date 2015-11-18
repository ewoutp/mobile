package toast;

import android.app.Activity;
import android.widget.Toast;
import java.lang.Runnable;

public abstract class ToastHelper {
    public static void show(final Activity activity, final String message, final int duration) {
        activity.runOnUiThread(new Runnable() {
            public void run() {
                Toast.makeText(activity, message, duration).show();
            }
        });
    }
}
