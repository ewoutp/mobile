// +build darwin
// +build arm arm64

package location

import (
	"log"
	"sync"
)

var (
	locationInitOnce sync.Once
	locationManager  *iosLocationManager
)

type iosLocationManager struct {
}

func locationManagerInit() {
	// TODO
	locationManager = &iosLocationManager{}
	log.Print("location initialized")
}

func (lm *iosLocationManager) RequestUpdates(updates chan Location, cancel <-chan struct{}) {
	// TODO
}

func getLocationManager() LocationManager {
	locationInitOnce.Do(locationManagerInit)
	return locationManager
}
