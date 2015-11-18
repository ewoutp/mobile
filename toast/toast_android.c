// +build android

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include "../internal/mobileinit/helper_android.h"

void toast_show(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *message, jint duration) {
	JavaVM* vm = (JavaVM*)java_vm;
	JNIEnv* env = (JNIEnv*)jni_env;
    jobject ctxObj = (jobject)ctx;

    jclass toastHelperClass = find_local_class(env, ctxObj, "toast/ToastHelper");
    jmethodID toastHelperShowID = find_static_method(env, toastHelperClass, "show", "(Landroid/app/Activity;Ljava/lang/String;I)V");

    jstring messageStr = (*env)->NewStringUTF(env, message);
	(*env)->CallStaticVoidMethod(env, toastHelperClass, toastHelperShowID, ctxObj, messageStr, duration);
}
