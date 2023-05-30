#include <iostream>
#include "Geometry.h"

using namespace Geometry;

Volume::Volume()
{
    vertices.resize(8);
}

Volume::Volume(std::vector<Point>&& vertices)
{
    if (vertices.size() == 8)
    {
        this->vertices = std::move(vertices);
    }
    else
    {
        std::cerr << "Incorrect number of vertices constructing Volume\n";
        exit(EXIT_FAILURE);
    }
}

Point Volume::getCenter() const
{
    double hx = vertices[1].x - vertices[0].x,
        hy = vertices[2].y - vertices[0].y,
        hz = vertices[4].z - vertices[0].z;
    return Point(vertices[0].x + hx / 2,
        vertices[0].y + hy / 2,
        vertices[0].z + hz / 2);
}

double Volume::getVolume() const
{
    return (vertices[rightBottomFront].x - vertices[leftBottomFront].x) *
        (vertices[leftBottomBack].y - vertices[leftBottomFront].y) *
        (vertices[leftTopFront].z - vertices[leftBottomFront].z);
}

Cube::Cube(std::vector<Point>&& vertices) : Volume(std::move(vertices))
{
    neighbors.resize(6, nullptr);
}

Cube::Cube(std::vector<Point>&& vertices, Vector3&& value)
{
    if (vertices.size() == 8)
    {
        this->vertices = std::move(vertices);
    }
    else
    {
        std::cerr << "Incorrect number of vertices constructing Volume\n";
        exit(EXIT_FAILURE);
    }
    this->value = std::move(value);
    neighbors.resize(6, nullptr);
}

Cube::Cube(std::vector<Point>&& vertices, Vector3 value)
{
    if (vertices.size() == 8)
    {
        this->vertices = std::move(vertices);
    }
    else
    {
        std::cerr << "Incorrect number of vertices constructing Volume\n";
        exit(EXIT_FAILURE);
    }
    this->value = value;
    neighbors.resize(6, nullptr);
}

int Cube::isCubeNeighbor(const Geometry::Cube& other) const
{
    return false;
}

bool Geometry::isPointInsideVolume(const Point& point, const Volume& volume)
{
    if (
        point.x >= volume.vertices[leftBottomFront].x - eps &&
        point.x <= volume.vertices[rightBottomFront].x + eps && 
        point.y >= volume.vertices[leftBottomFront].y - eps &&
        point.y <= volume.vertices[leftBottomBack].y + eps &&
        point.z >= volume.vertices[leftBottomFront].z - eps &&
        point.z <= volume.vertices[leftTopFront].z + eps
        )
    {
        return true;
    }
    return false;
}

bool Geometry::isVolumeInsideVolume(const Volume& volume1, const Volume& volume2)
{
    for (const auto& point : volume1.vertices)
    {
        if (!isPointInsideVolume(point, volume2))
        {
            return false;
        }
    }
    return true;
}
