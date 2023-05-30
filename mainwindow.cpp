#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "back/Input.h"
#include "back/DirectProblem.h"
#include "back/InverseProblem.h"
#include "back/Solver.h"
#include "blockpainter.h"
#include "dialoginputareaandanomaly.h"
#include "qvalidator.h"
#include <QFileDialog>
#include <QSplineSeries>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDoubleValidator *dVald = new QDoubleValidator(this);
    ui->lineEditAlpha->setValidator(dVald);
    ui->lineEditGamma0->setValidator(dVald);
    ui->lineEditGammaD->setValidator(dVald);
    ui->lineEditGammaPDiff->setValidator(dVald);
    ui->lineEditGammaFiDiff->setValidator(dVald);
}

MainWindow::~MainWindow()
{
    if (QFile::exists("area.txt"))
        QFile::remove("area.txt");
    if (QFile::exists("recievers.txt"))
        QFile::remove("recievers.txt");
    if (QFile::exists("anomaly.txt"))
        QFile::remove("anomaly.txt");
    if (QFile::exists("temp.txt"))
        QFile::remove("temp.txt");
    delete ui;
}

void MainWindow::on_ButtonOpenDialogAreaAndAnomaly_clicked()
{
    QWidget *dialog = new DialogInputAreaAndAnomaly(this);
    dialog->show();
}


void MainWindow::on_ButtonSolve_clicked()
{
    if (!QFile::exists("area.txt") || !QFile::exists("recievers.txt") || !QFile::exists("anomaly.txt"))
    {
        std::cerr << "error while solving: no temp files\n";
        return;
    }

    std::tie(volume, cubes, sizes) = Input::inputArea("area.txt");
    Input::inputAnomalies("anomaly.txt", cubes);
    recievers = Input::inputRecievers("recievers.txt");
    Input::setNeighbors(cubes, sizes);

    DirectProblem::calculateRecieversValues(recievers, cubes);

    auto a = InverseProblem::calculateMatrixA(recievers, cubes, ui->lineEditAlpha->text().toDouble());
    auto b = InverseProblem::calculateVectorB(recievers, cubes);
    cubesSolved = cubes;
    solution = Solver::gauss(a, b);

    for (size_t i = 0; i < solution.size(); i += 3)
    {
        cubesSolved[i / 3].value.x = solution[i + 0];
        cubesSolved[i / 3].value.y = solution[i + 1];
        cubesSolved[i / 3].value.z = solution[i + 2];
    }

    std::vector<Reciever> newRecievers = recievers;
    DirectProblem::calculateRecieversValues(newRecievers, cubesSolved);
    double fi = InverseProblem::calculateFi(recievers, newRecievers);

    if (ui->checkBoxUseGamma->isChecked())
    {
        double newFi = fi;
        bool gammasChanged = true;
        gammas.clear();
        gammas.resize(solution.size(), ui->lineEditGamma0->text().toDouble());

        auto aCopy = a;
        size_t counter = 0;
        while (std::abs(std::log10(fi) - std::log10(newFi)) <= std::log10(ui->lineEditGammaFiDiff->text().toDouble()) && gammasChanged)
        {
            counter++;
            gammasChanged = false;
            InverseProblem::updateMatrixA(aCopy, cubes, gammas);
            solution = Solver::gauss(aCopy, b);

            for (size_t i = 0; i < solution.size(); i += 3)
            {
                cubesSolved[i / 3].value.x = solution[i + 0];
                cubesSolved[i / 3].value.y = solution[i + 1];
                cubesSolved[i / 3].value.z = solution[i + 2];
            }
            DirectProblem::calculateRecieversValues(newRecievers, cubesSolved);
            newFi = InverseProblem::calculateFi(recievers, newRecievers);
            bool updated = InverseProblem::updateGammas(cubes, gammas,
                            ui->lineEditGammaPDiff->text().toDouble(), ui->lineEditGammaD->text().toDouble());
            if (updated)
                gammasChanged = true;
            if(counter == 500)
                break;
        }
        fi = newFi;
    }

    QLineSeries *solvedseries = new QLineSeries();
    for (size_t i = 0; i < newRecievers.size(); i += 3)
    {
        solvedseries->append(newRecievers[i].point.x, newRecievers[i].value.x);
    }
    solvedseries->setName("Полученное решение");
    QLineSeries *trueseries = new QLineSeries();
    for (size_t i = 0; i < recievers.size(); i += 3)
    {
        trueseries->append(recievers[i].point.x, recievers[i].value.x);
    }
    trueseries->setName("Истинное решение");
    QChart *chart = new QChart();
    chart->addSeries(solvedseries);
    chart->addSeries(trueseries);
    chart->setTitle("График компоненты Bx");
    chart->createDefaultAxes();
    //chart->legend()->hide();
    ui->widgetChart->setChart(chart);

    auto pixmapBlocks = BlockPainter::DrawBlocks(ui->labelBlocks->width(), ui->labelBlocks->height(), volume, cubesSolved, true);
    ui->labelBlocks->setPixmap(pixmapBlocks);
    ui->statusbar->showMessage("Значение функционала Φ: " + QString::number(fi));

}

