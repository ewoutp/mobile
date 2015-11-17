package location;

import android.location.Location;
import android.location.LocationListener;
import android.os.Bundle;

public class NativeLocationListener implements LocationListener {
    @Override
    public native void onLocationChanged (Location location);

    @Override
    public native void onProviderDisabled (String provider);

    @Override
    public native void onProviderEnabled (String provider);

    @Override
    public native void onStatusChanged (String provider, int status, Bundle extras);
}
