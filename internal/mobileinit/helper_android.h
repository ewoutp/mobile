// +build android

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>

#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, "Go", __VA_ARGS__)
#define LOG_FATAL(...) __android_log_print(ANDROID_LOG_FATAL, "Go", __VA_ARGS__)

static jclass find_class(JNIEnv *env, const char *class_name) {
	jclass clazz = (*env)->FindClass(env, class_name);
	if (clazz == NULL) {
		(*env)->ExceptionClear(env);
		LOG_FATAL("cannot find %s", class_name);
		return NULL;
	}
	return clazz;
}

static jmethodID find_method(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	jmethodID m = (*env)->GetMethodID(env, clazz, name, sig);
	if (m == 0) {
		(*env)->ExceptionClear(env);
		LOG_FATAL("cannot find method %s %s", name, sig);
		return 0;
	}
	return m;
}

static jmethodID find_static_method(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	jmethodID m = (*env)->GetStaticMethodID(env, clazz, name, sig);
	if (m == 0) {
		(*env)->ExceptionClear(env);
		LOG_FATAL("cannot find static method %s %s", name, sig);
		return 0;
	}
	return m;
}

static jobject global_ref(JNIEnv *env, jobject obj) {
	return (*env)->NewGlobalRef(env, obj);
}

static jclass find_local_class(JNIEnv *env, jobject context, const char *className) {
    jclass contextClass = (*env)->GetObjectClass(env, context);
	jclass classClass = find_class(env, "java/lang/Class");
	jclass classLoaderClass = find_class(env, "java/lang/ClassLoader");
	jmethodID getClassLoaderID = find_method(env, classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
	jmethodID loadClassID = find_method(env, classLoaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

    jobject cl = (*env)->CallObjectMethod(env, contextClass, getClassLoaderID);
	jstring jClassName = (*env)->NewStringUTF(env, className);
	jclass clazz = (*env)->CallObjectMethod(env, cl, loadClassID, jClassName);
    if (clazz == NULL) {
		(*env)->ExceptionClear(env);
		LOG_FATAL("cannot find %s", className);
		return NULL;
	}
    return clazz;
}
