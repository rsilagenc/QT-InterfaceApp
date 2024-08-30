#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "datahandler.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void setColumnWidths();
    void on_pushButton_clicked();
    void on_tableWidget_cellChanged(int row, int column);
    void populateTable();
    void retrieveTableValues();
    void keyGenerator();
    void IV();
    void HWIDArray();
private:
    Ui::MainWindow *ui;
    std::unique_ptr<dataHandler> uniqueDataHandler;


};
#endif // MAINWINDOW_H
