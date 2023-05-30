#define _USE_MATH_DEFINES

#include <cmath>
#include "DirectProblem.h"

void DirectProblem::calculateRecieversValues(std::vector<Reciever>& recievers,
    const std::vector<Geometry::Cube>& cubes)
{
    for (auto& reciever : recievers)
    {
        reciever.value = std::move(Geometry::Vector3(0, 0, 0));
        for (const auto& cube : cubes)
        {
            double x = reciever.point.x - cube.getCenter().x,
                y = reciever.point.y - cube.getCenter().y,
                z = reciever.point.z - cube.getCenter().z;
            double r = std::sqrt(x * x + y * y + z * z);
            double rSqareMinus2 = 1 / (r * r);
            double front = cube.getVolume() / (4 * M_PI * std::pow(r, 3));

            reciever.value.x += front * (cube.value.x * (3 * x * x * rSqareMinus2 - 1) +
                cube.value.y * (3 * x * y * rSqareMinus2) +
                cube.value.z * (3 * x * z * rSqareMinus2));
            reciever.value.y += front * (cube.value.x * (3 * x * y * rSqareMinus2) +
                cube.value.y * (3 * y * y * rSqareMinus2 - 1) +
                cube.value.z * (3 * y * z * rSqareMinus2));
            reciever.value.z += front * (cube.value.x * (3 * x * z * rSqareMinus2) +
                cube.value.y * (3 * y * z * rSqareMinus2) +
                cube.value.z * (3 * z * z * rSqareMinus2 - 1));
        }
    }
}
