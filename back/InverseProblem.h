#pragma once
#include<vector>
#include "Geometry.h"
#include "Recievers.h"

namespace InverseProblem
{
    std::vector<std::vector<double>> calculateMatrixA(const std::vector<Reciever>& recievers,
        const std::vector<Geometry::Cube>& cubes, double alpha);
    std::vector<double> calculateVectorB(const std::vector<Reciever>& recievers,
        const std::vector<Geometry::Cube>& cubes);
    void updateMatrixA(std::vector<std::vector<double>>& matrixA,
        const std::vector<Geometry::Cube>& cubes, std::vector<double>& gammas);

    bool updateGammas(const std::vector<Geometry::Cube>& cubes, std::vector<double>& gammas, double diff, double value);

    double calculateFi(const std::vector<double>& S, const std::vector<double>& SCap);
    double calculateFi(const std::vector<Reciever>& S, const std::vector<Reciever>& SCap);
}
