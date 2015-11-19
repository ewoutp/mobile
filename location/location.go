package location

import (
	"math"
)

type DistanceUnit float64

const (
	DistanceUnitMeter     = DistanceUnit(6367000.0)
	DistanceUnitKilometer = DistanceUnit(6367.0)
	DistanceUnitMile      = DistanceUnit(3956.0)
)

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

const (
	d2r = math.Pi / 180.0
)

// Distance calculated the distance between l and lat,lon location in the given unit.
func (l Location) Distance(lat, lon float64, unit DistanceUnit) float64 {
	dlong := (lon - l.Longitude) * d2r
	dlat := (lat - l.Latitude) * d2r
	a := math.Pow(math.Sin(dlat/2.0), 2) + math.Cos(lat*d2r)*math.Cos(l.Latitude*d2r)*math.Pow(math.Sin(dlong/2.0), 2)
	c := 2 * math.Atan2(math.Sqrt(a), math.Sqrt(1-a))
	return float64(unit) * c
}
