#include "geology.h"

/**
 * @brief calculate arc distance
 *
 * @param dLongitude_degree0
 * @param dLatitude_degree0
 * @param dLongitude_degree1
 * @param dLatitude_degree1
 * @return float
 */
float calculate_distance_based_on_lon_lat_degree(const float dLongitude_degree0,
                                                 const float dLatitude_degree0,
                                                 const float dLongitude_degree1,
                                                 const float dLatitude_degree1)
{

    // convert decimal degrees to radians

    const float dLongitude_radian0 = convert_degree_to_radian(dLongitude_degree0);
    const float dLatitude_radian0 = convert_degree_to_radian(dLatitude_degree0);

    const float dLongitude_radian1 = convert_degree_to_radian(dLongitude_degree1);
    const float dLatitude_radian1 = convert_degree_to_radian(dLatitude_degree1);

    // haversine formula
    const float dDelta_longitude_radian = dLongitude_radian1 - dLongitude_radian0;
    const float dDelta_latitude_radian = dLatitude_radian1 - dLatitude_radian0;

    const float a = sin(dDelta_latitude_radian / 2) * sin(dDelta_latitude_radian / 2) + cos(dLatitude_radian0) * cos(dLatitude_radian1) * sin(dDelta_longitude_radian / 2) * sin(dDelta_longitude_radian / 2);
    const float c = 2 * asin(sqrt(a));

    const float dDistance = c * dRadius_earth;
    return dDistance;
}

/**
 * @brief calculate xyz location using lat/lon and elevation
 *
 * @param dLongitude_radian
 * @param dLatitude_radian
 * @param dElevation
 * @return std::array<float ,3>
 */
std::array<float, 3> calculate_location_based_on_lon_lat_radian(float dLongitude_radian, float dLatitude_radian, float dElevation)
{
    std::array<float, 3> aLocation;
    // see: http://www.mathworks.de/help/toolbox/aeroblks/llatoecefposition.html
    float f = 1.0 / 298.257223563; // # Flattening factor WGS84 Model
    float cosLat = cos(dLatitude_radian);
    float sinLat = sin(dLatitude_radian);
    float FF = (1.0 - f) * (1.0 - f);
    float C = 1 / sqrt(cosLat * cosLat + FF * sinLat * sinLat);
    float S = C * FF;

    float x = (dRadius_earth * C + dElevation) * cosLat * cos(dLongitude_radian);
    float y = (dRadius_earth * C + dElevation) * cosLat * sin(dLongitude_radian);
    float z = (dRadius_earth * S + dElevation) * sinLat;

    aLocation[0] = x;
    aLocation[1] = y;
    aLocation[2] = z;
    return aLocation;
}