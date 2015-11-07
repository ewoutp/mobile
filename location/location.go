package location

type Location struct {
	Latitude  float64 // In degrees
	Longitude float64 // In degrees
	Speed     float32
}

type LocationManager interface {
	RequestUpdates(updates chan Location, cancel <-chan struct{})
}

func GetLocationManager() LocationManager {
	lm := getLocationManager()
	return lm
}
