#pragma once

#include <memory>
#include <sys/time.h>

enum class PosType { fixed, sunrel, moonpos, random, orbit };

class Coordinate
{
public:
    Coordinate(PosType t) : type(t) {}
    ~Coordinate() = default;
    const PosType getType() { return type; };
private:
    const PosType type;

};

using TCoordinatePtr = std::shared_ptr<Coordinate>;

class GeoCoordinate : public Coordinate
{
public:
    GeoCoordinate(double latitude, double longitude, PosType t = PosType::sunrel)
     : Coordinate(t),
       latitude(latitude),
       longitude(longitude) {}

    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }

    void setLatitude(double lat) { latitude = lat; }
    void setLongitude(double lon) { longitude = lon; }

private:
    double latitude = 0;
    double longitude = 0;
};

using TGeoCoordinatePtr = std::shared_ptr<GeoCoordinate>;

class OrbitCoordinate : public GeoCoordinate
{
public:
    OrbitCoordinate(double period, double inclin, double shift, PosType t = PosType::orbit)
     : GeoCoordinate(0, 0, t),
       orbit_period(period),
       orbit_inclin(inclin),
       orbit_shift(shift) {}

    double getPeriode() const { return orbit_period; }
    double getIncline() const { return orbit_period; }
    double getShifte() const { return orbit_shift; }
    void computePosition(time_t);

private:
    double orbit_period = 0;
    double orbit_inclin = 0;
    double orbit_shift = 0;
};

using TOrbitCoordinatePtr = std::shared_ptr<OrbitCoordinate>;
