// +build android

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "_cgo_export.h"
#include "../internal/mobileinit/helper_android.h"

jobject location_manager;
jobject locationListener;
jobject locationHandlerThread;

jclass locationClass; // android/location/Location.class
jmethodID getLatitudeID; //android/location/Location.getLatitude()
jmethodID getLongitudeID; //android/location/Location.getLongitude()
jmethodID getSpeedID; //android/location/Location.getSpeed()
jclass locationManagerClass; // android/location/LocationManager.class
jclass looperClass; // android/os/Looper.class
jclass handlerThreadClass; // android/os/HandlerThread
jmethodID handlerThreadCtor; // android/os/HandlerThread.ctor(String)
jmethodID handlerThreadGetLooper; // android/os/HandlerThread.getLooper()
jmethodID requestLocationUpdatesID; // android/location/LocationManager.requestLocationUpdates
jmethodID removeUpdatesID; // android/location/LocationManager.removeUpdates

void location_manager_init(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx) {
	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
	// Equivalent to:
	//	locationManager = ctx.getResources().getSystemService("location");
	jclass ctx_clazz = find_class(env, "android/content/Context");
	jmethodID getSystemService_id = find_method(env, ctx_clazz, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jstring locationStr = (*env)->NewStringUTF(env, "location");
	jobject lm = (*env)->CallObjectMethod(env, (jobject)ctx, getSystemService_id, locationStr);
	// Pin the LocationManager.
	location_manager = (*env)->NewGlobalRef(env, lm);

	// Preload other classes
	locationClass = global_ref(env, find_class(env, "android/location/Location"));
	getLatitudeID = find_method(env, locationClass, "getLatitude", "()D");
	getLongitudeID = find_method(env, locationClass, "getLongitude", "()D");
	getSpeedID = find_method(env, locationClass, "getSpeed", "()F");
	locationManagerClass = global_ref(env, find_class(env, "android/location/LocationManager"));
	looperClass = global_ref(env, find_class(env, "android/os/Looper"));
	handlerThreadClass = global_ref(env, find_class(env, "android/os/HandlerThread"));
	handlerThreadCtor = find_method(env, handlerThreadClass, "<init>", "(Ljava/lang/String;)V");
	handlerThreadGetLooper = find_method(env, handlerThreadClass, "getLooper", "()Landroid/os/Looper;");
	requestLocationUpdatesID = find_method(env, locationManagerClass, "requestLocationUpdates", "(Ljava/lang/String;JFLandroid/location/LocationListener;Landroid/os/Looper;)V");
	removeUpdatesID = find_method(env, locationManagerClass, "removeUpdates", "(Landroid/location/LocationListener;)V");

	// Create location handler thread
	jstring threadName = (*env)->NewStringUTF(env, "go-location");
	locationHandlerThread = global_ref(env, (*env)->NewObject(env, handlerThreadClass, handlerThreadCtor, threadName));
	jmethodID startID = find_method(env, handlerThreadClass, "start", "()V");
	(*env)->CallVoidMethod(env, locationHandlerThread, startID);
}

void location_manager_requestLocationUpdates(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *provider, jlong minTime, jfloat minDistance) {
	if (locationListener) {
		// Already initialized
		return;
	}
	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
	jobject ctxObj = (jobject)ctx;

	jclass cls = find_local_class(env, ctxObj, "location/NativeLocationListener");
	jmethodID ctorID = find_method(env, cls, "<init>", "()V");
	locationListener = global_ref(env, (*env)->NewObject(env, cls, ctorID));

	jobject looper = (*env)->CallObjectMethod(env, locationHandlerThread, handlerThreadGetLooper);

	jstring providerStr = (*env)->NewStringUTF(env, provider);
	 (*env)->CallVoidMethod(env, location_manager, requestLocationUpdatesID, providerStr, minTime, minDistance, locationListener, looper);
}

void location_manager_stopLocationUpdates(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx) {
	if (!locationListener) {
		// No current listener
		return;
	}

	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
	jobject ctxObj = (jobject)ctx;

	 (*env)->CallVoidMethod(env, location_manager, removeUpdatesID, locationListener);
	 locationListener = NULL;
}

// Class:     location_NativeLocationListener
// Method:    onLocationChanged
// Signature: (Landroid/location/Location;)V
JNIEXPORT void JNICALL Java_location_NativeLocationListener_onLocationChanged(JNIEnv * env, jobject this, jobject location) {
	locationData data;
	data.latitude = (*env)->CallDoubleMethod(env, location, getLatitudeID);
	data.longitude = (*env)->CallDoubleMethod(env, location, getLongitudeID);
	data.speed = (*env)->CallFloatMethod(env, location, getSpeedID);
	onLocationChanged(&data);
}

// Class:     location_NativeLocationListener
// Method:    onProviderDisabled
// Signature: (Ljava/lang/String;)V
JNIEXPORT void JNICALL Java_location_NativeLocationListener_onProviderDisabled(JNIEnv *env, jobject this, jstring provider) {
	const char *providerStr = (*env)->GetStringUTFChars(env, provider, NULL);
	onProviderDisabled(providerStr);
	(*env)->ReleaseStringUTFChars(env, provider, providerStr);
}

// Class:     location_NativeLocationListener
// Method:    onProviderEnabled
// Signature: (Ljava/lang/String;)V
JNIEXPORT void JNICALL Java_location_NativeLocationListener_onProviderEnabled(JNIEnv *env, jobject this, jstring provider) {
	const char *providerStr = (*env)->GetStringUTFChars(env, provider, NULL);
	onProviderEnabled(providerStr);
	(*env)->ReleaseStringUTFChars(env, provider, providerStr);
}

// Class:     location_NativeLocationListener
// Method:    onStatusChanged
// Signature: (Ljava/lang/String;ILandroid/os/Bundle;)V
JNIEXPORT void JNICALL Java_location_NativeLocationListener_onStatusChanged(JNIEnv *env, jobject this, jstring provider, jint status, jobject extras) {
	const char *providerStr = (*env)->GetStringUTFChars(env, provider, NULL);
	onStatusChanged(providerStr, status);
	(*env)->ReleaseStringUTFChars(env, provider, providerStr);
}
