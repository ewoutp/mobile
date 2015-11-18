package location

/*
#cgo LDFLAGS: -landroid -llog
#include <jni.h>
#include <stdlib.h>

void location_manager_init(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx);
void location_manager_requestLocationUpdates(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx, const char *provider, jlong minTime, jfloat minDistance);
void location_manager_stopLocationUpdates(uintptr_t java_vm, uintptr_t jni_env, uintptr_t ctx);

typedef struct {
	double latitude;
	double longitude;
	float speed;
} locationData;

*/
import "C"
import (
	"log"
	"sync"
	"unsafe"

	"golang.org/x/mobile/internal/mobileinit"
)

var (
	locationInitOnce sync.Once
	locationManager  *androidLocationManager
)

type androidLocationManager struct {
}

func locationManagerInit() {
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		C.location_manager_init(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx))
		return nil
	})
	if err != nil {
		log.Fatalf("location: %v", err)
	}
	locationManager = &androidLocationManager{}
	log.Print("location initialized")
}

func requestLocationUpdates(provider string, minTime int64, minDistance float32) {
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		cProvider := C.CString(provider)
		defer C.free(unsafe.Pointer(cProvider))
		C.location_manager_requestLocationUpdates(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx), cProvider, C.jlong(minTime), C.jfloat(minDistance))
		return nil
	})
	if err != nil {
		log.Fatalf("requestLocationUpdates: %v", err)
	}
	log.Print("requestLocationUpdates succeeded")
}

func stopLocationUpdates() {
	err := mobileinit.RunOnJVM(func(vm, env, ctx uintptr) error {
		C.location_manager_stopLocationUpdates(C.uintptr_t(vm), C.uintptr_t(env), C.uintptr_t(ctx))
		return nil
	})
	if err != nil {
		log.Fatalf("stopLocationUpdates: %v", err)
	}
	log.Print("stopLocationUpdates succeeded")
}

func (lm *androidLocationManager) RequestUpdates(updates chan Location, cancel <-chan struct{}) {
	minTime := int64(1000)      // minimum time interval between location updates, in milliseconds
	minDistance := float32(1.0) // minimum distance between location updates, in meters
	requestLocationUpdates("gps", minTime, minDistance)
}

func getLocationManager() LocationManager {
	locationInitOnce.Do(locationManagerInit)
	return locationManager
}

//export onLocationChanged
func onLocationChanged(data *C.locationData) {
	log.Print("onLocationChanged %#v", data)
}

//export onProviderDisabled
func onProviderDisabled(provider *C.char) {
	log.Printf("onProviderDisabled('%s')", C.GoString(provider))
}

//export onProviderEnabled
func onProviderEnabled(provider *C.char) {
	log.Printf("onProviderEnabled('%s')", C.GoString(provider))
}

//export onStatusChanged
func onStatusChanged(provider *C.char, status C.jint) {
	log.Printf("onStatusChanged('%s', %v)", C.GoString(provider), status)
}
