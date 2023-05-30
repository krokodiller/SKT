#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdint>
#include "InverseProblem.h"

std::vector<std::vector<double>> calculateMatrixL(const std::vector<Reciever>& recievers,
    const std::vector<Geometry::Cube>& cubes)
{
    size_t k = cubes.size(), n = recievers.size();
    std::vector<std::vector<double>> L(3 * k, std::vector<double>(3 * n));

    for (size_t i = 0; i < k * 3; i += 3)
    {
        for (size_t j = 0; j < n * 3; j += 3)
        {
            double x = recievers[j / 3].point.x - cubes[i / 3].getCenter().x,
                y = recievers[j / 3].point.y - cubes[i / 3].getCenter().y,
                z = recievers[j / 3].point.z - cubes[i / 3].getCenter().z;
            double r = std::sqrt(x * x + y * y + z * z);
            double rSqareMinus2 = 1 / (r * r);
            double front = cubes[i / 3].getVolume() / (4 * M_PI * std::pow(r, 3));

            L[i][j] += front * (3 * x * x * rSqareMinus2 - 1);
            L[i + 1][j] += front * (3 * x * y * rSqareMinus2);
            L[i + 2][j] += front * (3 * x * z * rSqareMinus2);
            L[i][j + 1] += front * (3 * x * y * rSqareMinus2);
            L[i + 1][j + 1] += front * (3 * y * y * rSqareMinus2 - 1);
            L[i + 2][j + 1] += front * (3 * y * z * rSqareMinus2);
            L[i][j + 2] += front * (3 * x * z * rSqareMinus2);
            L[i + 1][j + 2] += front * (3 * y * z * rSqareMinus2);
            L[i + 2][j + 2] += front * (3 * z * z * rSqareMinus2 - 1);
        }
    }
    return L;
}

std::vector<std::vector<double>> InverseProblem::calculateMatrixA(const std::vector<Reciever>& recievers, 
    const std::vector<Geometry::Cube>& cubes, double alpha)
{
    auto L = calculateMatrixL(recievers, cubes);

    size_t k = cubes.size(), n = recievers.size();
    std::vector<std::vector<double>> A(3 * k, std::vector<double>(3 * k));
#pragma omp parallel for
    for (int64_t i = 0; i < k * 3; i++)
    {
        A[i][i] += alpha;
        for (size_t j = 0; j < k * 3; j++)
        {
            for (size_t l = 0; l < n * 3; l++)
            {
                A[i][j] += L[i][l] * L[j][l];
            }
        }
    }

    return A;
}

std::vector<double> InverseProblem::calculateVectorB(const std::vector<Reciever>& recievers,
    const std::vector<Geometry::Cube>& cubes)
{
    auto L = calculateMatrixL(recievers, cubes);

    size_t k = cubes.size(), n = recievers.size();
    std::vector<double> b(3 * k);
#pragma omp parallel for
    for (int64_t i = 0; i < k * 3; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            b[i] += L[i][j * 3 + 0] * recievers[j].value.x;
            b[i] += L[i][j * 3 + 1] * recievers[j].value.y;
            b[i] += L[i][j * 3 + 2] * recievers[j].value.z;
        }
    }

    return b;
}

void InverseProblem::updateMatrixA(std::vector<std::vector<double>>& matrixA,
    const std::vector<Geometry::Cube>& cubes, std::vector<double>& gammas)
{
    for (size_t k = 0; k < cubes.size(); k++)
    {
        size_t mCount = 0;
        std::array<double, 3> diagonal{};
        for (size_t neighborN = 0; neighborN < 6; neighborN++)
        {
            if (cubes[k].neighbors[neighborN] != nullptr)
            {
                size_t m = cubes[k].neighbors[neighborN]->number;
                if (k != m)
                {
                    matrixA[k * 3 + 0][m * 3 + 0] -= (gammas[k * 3 + 0] + gammas[m * 3 + 0]);
                    matrixA[k * 3 + 1][m * 3 + 1] -= (gammas[k * 3 + 1] + gammas[m * 3 + 1]);
                    matrixA[k * 3 + 2][m * 3 + 2] -= (gammas[k * 3 + 2] + gammas[m * 3 + 2]);

                    diagonal[0] += gammas[m * 3 + 0];
                    diagonal[1] += gammas[m * 3 + 1];
                    diagonal[2] += gammas[m * 3 + 2];
                    mCount++;
                }
            }
        }
        matrixA[k * 3 + 0][k * 3 + 0] += mCount * gammas[k * 3 + 0] + diagonal[0];
        matrixA[k * 3 + 1][k * 3 + 1] += mCount * gammas[k * 3 + 1] + diagonal[1];
        matrixA[k * 3 + 2][k * 3 + 2] += mCount * gammas[k * 3 + 2] + diagonal[2];
    }
}


bool InverseProblem::updateGammas(const std::vector<Geometry::Cube>& cubes, std::vector<double>& gammas, double diff, double value)
{
    bool updated = false;
    for (size_t cubeN = 0; cubeN < cubes.size(); cubeN++)
    {
        for (size_t neighborN = 0; neighborN < 6; neighborN++)
        {
            if (cubes[cubeN].neighbors[neighborN] != nullptr)
            {
                if (cubes[cubeN].value.x / cubes[cubeN].neighbors[neighborN]->value.x > diff)
                {
                    updated = true;
                    gammas[cubeN * 3 + 0] *= value;
                }
                if (cubes[cubeN].value.y / cubes[cubeN].neighbors[neighborN]->value.y > diff)
                {
                    updated = true;
                    gammas[cubeN * 3 + 1] *= value;
                }
                if (cubes[cubeN].value.z / cubes[cubeN].neighbors[neighborN]->value.z > diff)
                {
                    updated = true;
                    gammas[cubeN * 3 + 2] *= value;
                }
            }
        }
    }
    return updated;
}


double InverseProblem::calculateFi(const std::vector<double>& S, const std::vector<double>& SCap)
{
    double fi = 0;
    for (size_t i = 0; i < S.size(); i++)
    {
        fi += std::pow(S[i] - SCap[i], 2);
    }
    return fi;
}

double InverseProblem::calculateFi(const std::vector<Reciever>& S, const std::vector<Reciever>& SCap)
{
    double fi = 0;
    for (size_t i = 0; i < S.size(); i++)
    {
        fi += std::pow(S[i].value.x - SCap[i].value.x, 2);
        fi += std::pow(S[i].value.y - SCap[i].value.y, 2);
        fi += std::pow(S[i].value.z - SCap[i].value.z, 2);
    }
    return fi;
}
