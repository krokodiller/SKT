#pragma once
#include <string>
#include <vector>
#include <tuple>
#include "Geometry.h"
#include "Recievers.h"
namespace Input
{
    static const std::string tarea {"AREA"};
    static const std::string trecievers {"RECIEVERS"};
    static const std::string tanomaly {"ANOMALY"};

    std::tuple<Geometry::Volume, std::vector<Geometry::Cube>, std::vector<size_t>> inputArea(std::string filename);
    void inputAnomalies(std::string filename, std::vector<Geometry::Cube>& cubes);
    std::vector<Reciever> inputRecievers(std::string filename);
    void setNeighbors(std::vector<Geometry::Cube>& cubes, const std::vector<size_t>& sizes);

    void inputFromSaveToTemp(std::string filename);
}
