// +build android

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "../internal/mobileinit/helper_android.h"

jint notification_notify(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *title, const char *text) {
	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
    jobject ctxObj = (jobject)ctx;

    jclass helperClass = find_local_class(env, ctxObj, "notification/NotificationHelper");
    //public static int notify(final Activity activity, final String title, final String text) {
    jmethodID helperNotifyID = find_static_method(env, helperClass, "notify", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)I");

    jstring titleStr = (*env)->NewStringUTF(env, title);
    jstring textStr = (*env)->NewStringUTF(env, text);
	return (*env)->CallStaticIntMethod(env, helperClass, helperNotifyID, ctxObj, titleStr, textStr);
}

void notification_close(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, jint id) {
	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
    jobject ctxObj = (jobject)ctx;

    jclass helperClass = find_local_class(env, ctxObj, "notification/NotificationHelper");
    jmethodID helperCloseID = find_static_method(env, helperClass, "close", "(Landroid/app/Activity;I)V");

	(*env)->CallStaticVoidMethod(env, helperClass, helperCloseID, ctxObj, id);
}
