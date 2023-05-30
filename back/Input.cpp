#include <fstream>
#include <iostream>
#include "Input.h"

std::tuple<Geometry::Volume, std::vector<Geometry::Cube>, std::vector<size_t>> Input::inputArea(std::string filename)
{
    std::ifstream r(filename);
    if (!r.good())
    {
        std::cerr << "Error opening " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    double xmin, xmax, ymin, ymax, zmin, zmax, xstep, ystep, zstep;
    size_t xnum, ynum, znum;

    r >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax;
    r >> xnum >> ynum >> znum;
    xstep = (xmax - xmin) / static_cast<double>(xnum);
    ystep = (ymax - ymin) / static_cast<double>(ynum);
    zstep = (zmax - zmin) / static_cast<double>(znum);

    std::vector<size_t> parameters(3);
    parameters[0] = xnum;
    parameters[1] = ynum;
    parameters[2] = znum;

    Geometry::Volume volume({
        Geometry::Point(xmin, ymin, zmin),
        Geometry::Point(xmax, ymin, zmin),
        Geometry::Point(xmin, ymax, zmin),
        Geometry::Point(xmax, ymax, zmin),
        Geometry::Point(xmin, ymin, zmax),
        Geometry::Point(xmax, ymin, zmax),
        Geometry::Point(xmin, ymax, zmax),
        Geometry::Point(xmax, ymax, zmax)
        });
    std::vector<Geometry::Cube> cubes;
    cubes.reserve(xnum * ynum * znum);

    for (size_t z = 0; z < znum; z++)
    {
        for (size_t y = 0; y < ynum; y++)
        {
            for (size_t x = 0; x < xnum; x++)
            {
                Geometry::Cube cube({
                    Geometry::Point(xmin + xstep * x, ymin + ystep * y, zmin + zstep * z),
                    Geometry::Point(xmin + xstep * x + xstep, ymin + ystep * y, zmin + zstep * z),
                    Geometry::Point(xmin + xstep * x, ymin + ystep * y + ystep, zmin + zstep * z),
                    Geometry::Point(xmin + xstep * x + xstep, ymin + ystep * y + ystep, zmin + zstep * z),
                    Geometry::Point(xmin + xstep * x, ymin + ystep * y, zmin + zstep * z + zstep),
                    Geometry::Point(xmin + xstep * x + xstep, ymin + ystep * y, zmin + zstep * z + zstep),
                    Geometry::Point(xmin + xstep * x, ymin + ystep * y + ystep, zmin + zstep * z + zstep),
                    Geometry::Point(xmin + xstep * x + xstep, ymin + ystep * y + ystep, zmin + zstep * z + zstep)
                    });
                cubes.push_back(std::move(cube));
                cubes.back().number = cubes.size() - 1;
            }
        }
    }

    return { volume, cubes, parameters };
}

void Input::inputAnomalies(std::string filename, std::vector<Geometry::Cube>& cubes)
{
    std::ifstream r(filename);
    if (!r.good())
    {
        std::cerr << "Error opening " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    size_t numberOfAnomalies;
    r >> numberOfAnomalies;

    for (size_t anomalyNum = 0; anomalyNum < numberOfAnomalies; anomalyNum++)
    {
        double xmin, xmax, ymin, ymax, zmin, zmax, px, py, pz;
        r >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax;
        r >> px >> py >> pz;

        Geometry::Vector3 value(px, py, pz);
        Geometry::Volume volume({
            Geometry::Point(xmin, ymin, zmin),
            Geometry::Point(xmax, ymin, zmin),
            Geometry::Point(xmin, ymax, zmin),
            Geometry::Point(xmax, ymax, zmin),
            Geometry::Point(xmin, ymin, zmax),
            Geometry::Point(xmax, ymin, zmax),
            Geometry::Point(xmin, ymax, zmax),
            Geometry::Point(xmax, ymax, zmax)
            });


        for (auto& cube : cubes)
        {
            if (Geometry::isVolumeInsideVolume(cube, volume))
            {
                cube.value = value;
            }
        }
    }
}

std::vector<Reciever> Input::inputRecievers(std::string filename)
{
    std::ifstream r(filename);
    if (!r.good())
    {
        std::cerr << "Error opening " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    std::vector<Reciever> recievers;
    size_t numberOfRecievers;
    r >> numberOfRecievers;
    recievers.resize(numberOfRecievers);

    double min, max, step;
    r >> min >> max;
    step = (max - min) / static_cast<double>(numberOfRecievers);

    for (size_t i = 0; i < numberOfRecievers; i++)
    {
        recievers[i].point.x = min + step * i;
    }
    r.close();
    return recievers;
}

void Input::setNeighbors(std::vector<Geometry::Cube>& cubes, const std::vector<size_t>& sizes)
{
    int xnum = sizes[0], ynum = sizes[1], znum = sizes[2];
    for (int z = 0; z < znum; z++)
    {
        for (int y = 0; y < ynum; y++)
        {
            for (int x = 0; x < xnum; x++)
            {
                if (!((z) * ynum*xnum + (y) * xnum + (x - 1) < 0 || (z) * ynum*xnum + (y) * xnum + (x - 1) >= cubes.size()) && x != 0)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::left] =
                            &cubes[(z) * ynum*xnum + (y) * xnum + (x - 1)];
                if (!((z) * ynum*xnum + (y) * xnum + (x + 1) < 0 || (z) * ynum*xnum + (y) * xnum + (x + 1) >= cubes.size()) && x != xnum - 1)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::right] =
                            &cubes[(z) * ynum*xnum + (y) * xnum + (x + 1)];
                if (!((z) * ynum*xnum + (y - 1) * xnum + (x) < 0 || (z) * ynum*xnum + (y - 1) * xnum + (x) >= cubes.size()) && y != 0)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::front] =
                            &cubes[(z) * ynum*xnum + (y - 1) * xnum + (x)];
                if (!((z) * ynum*xnum + (y + 1) * xnum + (x) < 0 || (z) * ynum*xnum + (y + 1) * xnum + (x) >= cubes.size()) && y != ynum - 1)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::back] =
                            &cubes[(z) * ynum*xnum + (y + 1) * xnum + (x)];
                if (!((z - 1) * ynum*xnum + (y) * xnum + (x) < 0 || (z - 1) * ynum*xnum + (y) * xnum + (x) >= cubes.size()) && z != 0)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::bottom] =
                            &cubes[(z - 1) * ynum*xnum + (y) * xnum + (x)];
                if (!((z + 1) * ynum*xnum + (y) * xnum + (x) < 0 || (z + 1) * ynum*xnum + (y) * xnum + (x) >= cubes.size()) && z != znum - 1)
                    cubes[(z) * ynum*xnum + (y) * xnum + (x)].neighbors[Geometry::top] =
                            &cubes[(z + 1) * ynum*xnum + (y) * xnum + (x)];
            }
        }
    }
}

void Input::inputFromSaveToTemp(std::string filename)
{
    std::ifstream f(filename);
    if (!f.good())
    {
        std::cerr << "Error opening " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    std::string tag;
    f >> tag;
    if  (tag != tarea)
    {
        std::cerr << "Error reading " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    double xmin, xmax, ymin, ymax, zmin, zmax;
    size_t xnum, ynum, znum;
    double px, py, pz;
    double min, max;
    size_t n;
    size_t numberOfAnomalies;

    f >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax;
    f >> xnum >> ynum >> znum;

    std::ofstream of("area.txt");
    of << xmin << " " << xmax << "\n" << ymin << " " << ymax << "\n" <<
          zmin << " " << zmax << "\n" << xnum << " " << ynum << " " << znum;
    of.close();

    f >> tag;
    if  (tag != trecievers)
    {
        std::cerr << "Error reading " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    f >> n >> min >> max;

    of.open("recievers.txt");
    of << n << "\n" << min << " " << max;
    of.close();

    f >> tag;
    if  (tag != tanomaly)
    {
        std::cerr << "Error reading " << filename << ".\n";
        exit(EXIT_FAILURE);
    }

    f >> numberOfAnomalies;

    of.open("anomaly.txt");
    of << numberOfAnomalies << "\n\n";

    for (size_t anomalyNum = 0; anomalyNum < numberOfAnomalies; anomalyNum++)
    {
        double xmin, xmax, ymin, ymax, zmin, zmax, px, py, pz;
        f >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax;
        f >> px >> py >> pz;

        of << xmin << " " << xmax << "\n" << ymin << " " << ymax << "\n" <<
              zmin << " " << zmax << "\n" << px << " " << py << " " << pz << "\n";
     }
    of.close();
}


