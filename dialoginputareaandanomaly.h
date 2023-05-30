#ifndef DIALOGINPUTAREAANDANOMALY_H
#define DIALOGINPUTAREAANDANOMALY_H

#include <QDialog>

namespace Ui {
class DialogInputAreaAndAnomaly;
}

class DialogInputAreaAndAnomaly : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInputAreaAndAnomaly(QWidget *parent = nullptr);
    ~DialogInputAreaAndAnomaly();

private slots:
    void on_pushButtonOk_clicked();

    void on_pushButtonSave_clicked();

    void on_pushButtonOpen_clicked();

    void on_pushButtonRedraw_clicked();

private:
    Ui::DialogInputAreaAndAnomaly *ui;

    void saveToFile(QString filename);
    void openFromFile(QString filename);
    void drawBlankPicture();
};

#endif // DIALOGINPUTAREAANDANOMALY_H
