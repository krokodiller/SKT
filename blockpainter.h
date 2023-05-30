#ifndef BLOCKPAINTER_H
#define BLOCKPAINTER_H
#include "back/Geometry.h"
#include "qpixmap.h"

class BlockPainter
{
public:
    static QPixmap DrawBlocks(int width, int height, const Geometry::Volume& volume,
                           const std::vector<Geometry::Cube>& cubes, bool printValuesX = 0);
private:
    static QColor getGray(double min, double max, double value);
    static QColor getWhiteOrBlack(double min, double max, double value);
};

#endif // BLOCKPAINTER_H
