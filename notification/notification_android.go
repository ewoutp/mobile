package notification

/*
#cgo LDFLAGS: -landroid -llog
#include <jni.h>
#include <stdlib.h>

jint notification_notify(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *title, const char *text);
void notification_close(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, jint id);

*/
import "C"
import (
	"log"
	"unsafe"

	"golang.org/x/mobile/internal/mobileinit"
)

type androidNotification struct {
	id int
}

func Notify(title, text string) Notification {
	var id int
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		cTitle := C.CString(title)
		defer C.free(unsafe.Pointer(cTitle))
		cText := C.CString(text)
		defer C.free(unsafe.Pointer(cText))
		id = int(C.notification_notify(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx), cTitle, cText))
		return nil
	})
	if err != nil {
		log.Fatalf("Notify: %v", err)
	}
	return &androidNotification{
		id: id,
	}
}

func (an *androidNotification) Close() {
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		C.notification_close(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx), C.jint(an.id))
		return nil
	})
	if err != nil {
		log.Fatalf("Notification.Close: %v", err)
	}
}
