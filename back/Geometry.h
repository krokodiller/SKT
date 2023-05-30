#pragma once
#include <memory>
#include <vector>
namespace Geometry
{
    constexpr double eps = 1e-15;
    enum Vertices
    {
        leftBottomFront,
        rightBottomFront,
        leftBottomBack,
        rightBottomBack,
        leftTopFront,
        rightTopFront,
        leftTopBack,
        rightTopBack
    };

    class Point
    {
    public:
        Point() : x(0), y(0), z(0) {}
        Point(double x, double y, double z) : x(x), y(y), z(z) {}
        double x, y, z;
    };

    using Vector3 = Point;
 /*   _____________
 *  /|6          /|7    ^ z
 * /____________/ |     |  ^ y
 * |4|          |5|     | /
 * | |          | |     |/----> x
 * | |          | |
 * | |          | |
 * | |__________|_|
 * | /2         | /3
 * |/___________|/
 * 0            1
 */

    class Volume
    {
    public:
        Volume();
        Volume(std::vector<Point>&& vertices);
        Point getCenter() const;
        double getVolume() const;
        std::vector<Point> vertices;
    };

    class Cube : public Volume
    {
    public:
        Cube(std::vector<Point>&& vertices);
        Cube(std::vector<Point>&& vertices, Vector3 value);
        Cube(std::vector<Point>&& vertices, Vector3&& value);
        int isCubeNeighbor(const Geometry::Cube& other) const;
        Vector3 value;
        int number = 0;
        std::vector<Geometry::Cube*> neighbors;
    };

    enum neighbors
    {
        left,
        right,
        bottom,
        top,
        front,
        back
    };

    void setNeighbors(std::vector<Geometry::Cube>& cubes);

    bool isPointInsideVolume(const Point& point, const Volume& volume);
    bool isVolumeInsideVolume(const Volume& volume1, const Volume& volume2);
}
