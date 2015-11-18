package toast

/*
#cgo LDFLAGS: -landroid -llog
#include <jni.h>
#include <stdlib.h>

void toast_show(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *message, jint duration);

*/
import "C"
import (
	"log"
	"unsafe"

	"golang.org/x/mobile/internal/mobileinit"
)

func show(message string, long bool) {
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		cMessage := C.CString(message)
		defer C.free(unsafe.Pointer(cMessage))
		duration := C.jint(0) // TOAST_SHORT
		if long {
			duration = C.jint(1) // TOAST_LONG
		}
		C.toast_show(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx), cMessage, duration)
		return nil
	})
	if err != nil {
		log.Fatalf("showToast: %v", err)
	}
}
