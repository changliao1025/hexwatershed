#include "geology.h"

#include <array>

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
std::array<float, 3> calculate_location_based_on_lon_lat_radian(float dLongitude_radian,
                                                                float dLatitude_radian,
                                                                float dElevation)
{
    std::array<float, 3> aLocation;
    // see: http://www.mathworks.de/help/toolbox/aeroblks/llatoecefposition.html
    float f = 1.0 / 298.257223563; // # Flattening factor WGS84 Model
    float cosLat = cos(dLatitude_radian);
    float sinLat = sin(dLatitude_radian);
    float FF = (1.0 - f) * (1.0 - f);
    float C = 1 / sqrt(cosLat * cosLat + FF * sinLat * sinLat);
    float S = C * FF;

    const float x = (dRadius_earth * C + dElevation) * cosLat * cos(dLongitude_radian);
    const float y = (dRadius_earth * C + dElevation) * cosLat * sin(dLongitude_radian);
    const float z = (dRadius_earth * S + dElevation) * sinLat;

    aLocation[0] = x;
    aLocation[1] = y;
    aLocation[2] = z;
    return aLocation;
}

std::array<float ,3>  convert_longitude_latitude_to_sphere_3d(float dLongitude_radian,
float dLatitude_radian)
{
    float a = cos(dLatitude_radian) * cos(dLongitude_radian);
    float b = cos(dLatitude_radian) * sin(dLongitude_radian);
    float c = sin(dLatitude_radian);

    return {a, b, c};
}

/**
    * @brief calculate angle using lat/lon
    *
    * @param dLongitude_radian
    * @param dLatitude_radian
    * @return std::array<float ,3>

*/
float calculate_angle_between_lon_lat_radian(int iFlag_reverse, float dLongitude_radian0, float dLatitude_radian0,
                                             float dLongitude_radian1, float dLatitude_radian1,
                                             float dLongitude_radian2, float dLatitude_radian2)
{
    float dAngle = 0.0;

    std::array<float, 3> a3 = convert_longitude_latitude_to_sphere_3d(dLongitude_radian0, dLatitude_radian0);
    std::array<float, 3> b3 = convert_longitude_latitude_to_sphere_3d(dLongitude_radian1, dLatitude_radian1);
    std::array<float, 3> c3 = convert_longitude_latitude_to_sphere_3d(dLongitude_radian2, dLatitude_radian2);

    float a3vec[3], c3vec[3];
    if (iFlag_reverse == 0) // this is the case when A is the end point
    {
        for (int i = 0; i < 3; ++i)
        {
            a3vec[i] = a3[i] - b3[i];
            c3vec[i] = c3[i] - b3[i];
        }
    }
    else // this is the case when A is the start point
    {
        for (int i = 0; i < 3; ++i)
        {
            a3vec[i] = b3[i] - a3[i];
            c3vec[i] = c3[i] - b3[i];
        }
    }

    float dot = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        dot += a3vec[i] * c3vec[i];
    }

    float g[3];

    g[0] = a3vec[1] * c3vec[2] - a3vec[2] * c3vec[1];
    g[1] = a3vec[2] * c3vec[0] - a3vec[0] * c3vec[2];
    g[2] = a3vec[0] * c3vec[1] - a3vec[1] * c3vec[0];

    float det = 0.0;
    for (int i = 0; i < 3; ++i)
    {
        det += b3[i] * g[i];
    }

    float angle = atan2(det, dot);
    float f = angle * 180.0 / M_PI;
    if (f < 0)
    {
        f += 360.0;
    }
    dAngle = f;

    return dAngle;
}
