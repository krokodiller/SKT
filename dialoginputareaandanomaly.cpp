#include "dialoginputareaandanomaly.h"
#include "qpainter.h"
#include "ui_dialoginputareaandanomaly.h"
#include <iostream>
#include <string>
#include <QFileDialog>
#include <QIntValidator>
#include <QDoubleValidator>
#include "back/Input.h"
#include "blockpainter.h"

DialogInputAreaAndAnomaly::DialogInputAreaAndAnomaly(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInputAreaAndAnomaly)
{
    ui->setupUi(this);
    drawBlankPicture();
    QDoubleValidator *dVald = new QDoubleValidator(this);
    ui->lineEditX1->setValidator(dVald);
    ui->lineEditX2->setValidator(dVald);
    ui->lineEditY1->setValidator(dVald);
    ui->lineEditY2->setValidator(dVald);
    ui->lineEditZ1->setValidator(dVald);
    ui->lineEditZ2->setValidator(dVald);
    ui->lineEditRecieversBeginX->setValidator(dVald);
    ui->lineEditRecieversEndX->setValidator(dVald);
}

void DialogInputAreaAndAnomaly::saveToFile(QString filename)
{
    if (filename.isEmpty())
        return;
    QFile f(filename);
    f.open(QIODevice::WriteOnly);

    QTextStream out(&f);
    out << QString::fromStdString(Input::tarea) << "\n";
    out << ui->lineEditX1->displayText() << " " << ui->lineEditX2->displayText() << "\n";
    out << ui->lineEditY1->displayText() << " " << ui->lineEditY2->displayText() << "\n";
    out << ui->lineEditZ1->displayText() << " " << ui->lineEditZ2->displayText() << "\n";
    out << ui->lineEditXBlocks->displayText() << " " << ui->lineEditYBlocks->displayText() << " " <<
           ui->lineEditZBlocks->displayText() << "\n";

    out << QString::fromStdString(Input::trecievers) << "\n";
    out << ui->spinBoxNRecievers->value() << "\n";
    out << ui->lineEditRecieversBeginX->displayText() << " " << ui->lineEditRecieversEndX->displayText() << "\n";

    out << QString::fromStdString(Input::tanomaly) << "\n";
    out << ui->textEditInput->toPlainText() << "\n";

    f.close();
}

void DialogInputAreaAndAnomaly::drawBlankPicture()
{
    QPixmap pixmap(ui->labelPixmap->width(), ui->labelPixmap->height());
    QPainter painter(&pixmap);
    painter.setBrush(QBrush(Qt::white));
    painter.drawRect(0, 0, pixmap.width(), pixmap.height());
    ui->labelPixmap->setPixmap(pixmap);
}

void DialogInputAreaAndAnomaly::openFromFile(QString filename)
{
    if (filename.isEmpty())
        return;
    Input::inputFromSaveToTemp(filename.toStdString());

    QFile a("area.txt");
    a.open(QIODevice::ReadOnly);
    QTextStream ina(&a);
    QString xmin, xmax, ymin, ymax, zmin, zmax, nx, ny, nz;
    ina >> xmin >> xmax >> ymin >> ymax >> zmin >> zmax >> nx >> ny >> nz;
    ui->lineEditX1->setText(xmin);
    ui->lineEditX2->setText(xmax);
    ui->lineEditY1->setText(ymin);
    ui->lineEditY2->setText(ymax);
    ui->lineEditZ1->setText(zmin);
    ui->lineEditZ2->setText(zmax);
    ui->lineEditXBlocks->setText(nx);
    ui->lineEditYBlocks->setText(ny);
    ui->lineEditZBlocks->setText(nz);
    a.close();

    QFile an("anomaly.txt");
    an.open(QIODevice::ReadOnly);
    QTextStream inan(&an);
    QString anomaly;
    anomaly = inan.readAll();
    ui->textEditInput->setPlainText(anomaly);
    an.close();

    QFile r("recievers.txt");
    r.open(QIODevice::ReadOnly);
    QTextStream inr(&r);
    QString min, max, n;
    inr >> n >> min >> max;
    ui->spinBoxNRecievers->setValue(n.toInt());
    ui->lineEditRecieversBeginX->setText(min);
    ui->lineEditRecieversEndX->setText(max);
    r.close();
}

DialogInputAreaAndAnomaly::~DialogInputAreaAndAnomaly()
{
    delete ui;
}

void DialogInputAreaAndAnomaly::on_pushButtonOk_clicked()
{
    saveToFile("temp.txt");
    Input::inputFromSaveToTemp("temp.txt");
    this->close();
}


void DialogInputAreaAndAnomaly::on_pushButtonSave_clicked()
{
    QString filename = QFileDialog::getSaveFileName();
    saveToFile(filename);
}


void DialogInputAreaAndAnomaly::on_pushButtonOpen_clicked()
{
    QString filename = QFileDialog::getOpenFileName();
    openFromFile(filename);
}


void DialogInputAreaAndAnomaly::on_pushButtonRedraw_clicked()
{
    saveToFile("temp.txt");
    Input::inputFromSaveToTemp("temp.txt");
    auto [volume, cubes, sizes] = Input::inputArea("area.txt");
    Input::inputAnomalies("anomaly.txt", cubes);
    auto pixmap = BlockPainter::DrawBlocks(ui->labelPixmap->width(), ui->labelPixmap->height(), volume, cubes, true);

    /*
    const int xshift = 50, yshift = 30;
    const int xmin = 0, ymin = 0;
    const int xmax = ui->labelPixmap->width(), ymax = ui->labelPixmap->height();
    QPixmap pixmap(xmax, ymax);
    QPainter painter(&pixmap);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::TextAntialiasing);

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
        if (std::abs(cube.value.x) > eps || std::abs(cube.value.y) > eps || std::abs(cube.value.z) > eps)
        {
            painter.setBrush(QBrush(Qt::gray));
            painter.drawRect(rect);
            painter.setBrush(QBrush(Qt::white));
        }
        else
        {
            painter.drawRect(rect);
        }
    }

    //axes
    painter.setPen(QPen(Qt::black, 3));
    QLineF xline(xminShifted, yminShifted, xminShifted, ymaxShifted);
    QLineF yline(xminShifted, ymaxShifted, xmaxShifted, ymaxShifted);
    painter.drawLine(xline);
    painter.drawLine(yline);

    painter.setFont(QFont("Consolas"));
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
    */

    ui->labelPixmap->setPixmap(pixmap);

}

