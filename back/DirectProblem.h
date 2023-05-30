#pragma once
#include <string>
#include <vector>
#include "Geometry.h"
#include "Recievers.h"

namespace DirectProblem
{
    void calculateRecieversValues(std::vector<Reciever>& recievers,
        const std::vector<Geometry::Cube>& cubes);

}