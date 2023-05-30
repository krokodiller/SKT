#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "back/Geometry.h"
#include "back/Recievers.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ButtonOpenDialogAreaAndAnomaly_clicked();

    void on_ButtonSolve_clicked();

private:
    Ui::MainWindow *ui;
    Geometry::Volume volume;
    std::vector<Geometry::Cube> cubes;
    std::vector<Reciever> recievers;
    std::vector<double> solution;
    std::vector<double> trueSolution;
    std::vector<Geometry::Cube> cubesSolved;
    std::vector<size_t> sizes;
    std::vector<double> gammas;
};
#endif // MAINWINDOW_H
