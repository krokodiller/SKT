#include "blockpainter.h"
#include "qpainter.h"
#include "qpixmap.h"
#include "back/Geometry.h"
#include <vector>
#include <algorithm>

QPixmap BlockPainter::DrawBlocks(int width, int height, const Geometry::Volume& volume,
                              const std::vector<Geometry::Cube>& cubes, bool printValuesX)
{
    double maxXValue, minXValue;
    maxXValue = std::max_element(cubes.cbegin(), cubes.cend(), [](const Geometry::Cube& l, const Geometry::Cube& r){
            return l.value.x < r.value.x;
})->value.x;
    minXValue = std::min_element(cubes.cbegin(), cubes.cend(), [](const Geometry::Cube& l, const Geometry::Cube& r){
        return l.value.x < r.value.x;
})->value.x;

    const int xshift = 50, yshift = 30;
    const int xmin = 0, ymin = 0;
    const int xmax = width, ymax = height;
    QPixmap pixmap(xmax, ymax);
    QPainter painter(&pixmap);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.setFont(QFont("Consolas"));
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(xmin, ymin, xmax, ymax);
    painter.setPen(QPen(Qt::black));
    int xminShifted = xmin + xshift, xmaxShifted = xmax, yminShifted = ymin, ymaxShifted = ymax - yshift;
    const double xscale = (xmaxShifted - xminShifted) /
            (volume.vertices[Geometry::rightBottomFront].x - volume.vertices[Geometry::leftBottomFront].x);
    const double yscale = (ymaxShifted - yminShifted) /
            (volume.vertices[Geometry::leftTopFront].z - volume.vertices[Geometry::leftBottomFront].z);
    const double eps = 1e-15;
    //rectangles
    for (const auto& cube : cubes)
    {
        QRectF rect(
                xshift + (cube.vertices[Geometry::leftTopFront].x - volume.vertices[Geometry::leftBottomFront].x) * xscale,
                ymaxShifted - (cube.vertices[Geometry::leftTopFront].z - volume.vertices[Geometry::leftBottomFront].z) * yscale,
                (cube.vertices[Geometry::rightTopFront].x - volume.vertices[Geometry::leftBottomFront].x) * xscale -
                    (cube.vertices[Geometry::leftTopFront].x - volume.vertices[Geometry::leftBottomFront].x) * xscale,
                (cube.vertices[Geometry::leftTopFront].z - volume.vertices[Geometry::leftBottomFront].z) * yscale -
                    (cube.vertices[Geometry::leftBottomFront].z - volume.vertices[Geometry::leftBottomFront].z) * yscale
                );
        painter.setBrush(QBrush(getGray(minXValue, maxXValue, cube.value.x)));
        painter.drawRect(rect);
        painter.setBrush(QBrush(Qt::white));
        if (printValuesX)
        {
            painter.setPen(getWhiteOrBlack(minXValue, maxXValue, cube.value.x));
            painter.drawText(rect, Qt::AlignCenter, QString::number(cube.value.x, 'g', 4));
            painter.setPen(QPen(Qt::black));
        }
    }

    //axes
    painter.setPen(QPen(Qt::black, 3));
    QLineF xline(xminShifted, yminShifted, xminShifted, ymaxShifted);
    QLineF yline(xminShifted, ymaxShifted, xmaxShifted, ymaxShifted);
    painter.drawLine(xline);
    painter.drawLine(yline);

    //x axis
    const double textShiftY = 15;
    const double textShiftX = 25;
    const int numberOfPoints = 10;
    for (int i = 0; i <= numberOfPoints; i++)
    {
        double x = xshift + i * ((xmaxShifted - xminShifted) / numberOfPoints);
        double text =  i * ((volume.vertices[Geometry::rightBottomFront].x - volume.vertices[Geometry::leftBottomFront].x) /
                numberOfPoints) + volume.vertices[Geometry::leftBottomFront].x;
        QPointF p(x - textShiftX, ymaxShifted + textShiftY);
        painter.drawText(p, QString::number(text, 'g', 4));
    }

    //y axis
    const double additionalTextShiftX = 10;
    for (int i = 0; i <= numberOfPoints; i++)
    {
        double y = i * ((ymaxShifted - yminShifted) / numberOfPoints);
        double text =  i * ((volume.vertices[Geometry::leftBottomFront].z - volume.vertices[Geometry::leftTopFront].z) /
                numberOfPoints) + volume.vertices[Geometry::leftTopFront].z;
        QPointF p(xminShifted - textShiftX - additionalTextShiftX, y);
        painter.drawText(p, QString::number(text, 'g', 4));
    }

    return pixmap;
}

QColor BlockPainter::getGray(double min, double max, double value)
{
    int numberOfShades = 10;
    double range = max - min;
    double norm = (value - min) / range;
    norm = 1 - norm;
    int colorByte = norm * 255;
    return QColor(colorByte, colorByte, colorByte);
}

QColor BlockPainter::getWhiteOrBlack(double min, double max, double value)
{
    int numberOfShades = 10;
    double range = max - min;
    double norm = (value - min) / range;
    norm = 1 - norm;
    if (norm < 0.5)
        return QColor(255, 255, 255);
    else
        return QColor(0, 0, 0);
}
