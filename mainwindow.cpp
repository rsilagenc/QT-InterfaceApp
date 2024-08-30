#include "mainwindow.h"
#include "rand.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QString>
#include <QMessageBox>
#include <QHeaderView>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    uniqueDataHandler = std::make_unique<dataHandler>();
    uniqueDataHandler->content.DateOfExpiry = QDateTime::currentDateTime().addDays(30);
    populateTable();
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::on_tableWidget_cellChanged);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    retrieveTableValues();
    uniqueDataHandler->content.DateOfExpiry = QDateTime::currentDateTime().addDays(30);
    keyGenerator();
    IV();

}

// qbytearray hex halinde tut
void MainWindow::retrieveTableValues()
{
    QTableWidgetItem *item1 = ui->tableWidget->item(0, 0);
    QString cellValue1 = item1->text();
    uniqueDataHandler->content.cellValueArray = cellValue1.toUtf8().toHex();
    if(uniqueDataHandler->content.cellValueArray.at(0) <= '\x65' && uniqueDataHandler->content.cellValueArray.at(0) <= '\x75' )
    {
        qDebug() << "Cell value:" << uniqueDataHandler->content.cellValueArray;
    }
    else
    {
        QMessageBox::warning(this,"Input Error","Please enter a valid number for ID." );
    }

 QTableWidgetItem *item3 = ui->tableWidget->item(2, 0);
 if (item3)
 {
     QString cellValue3 = item3->text();
     // Convert the cell value to the hex value
     QByteArray hwidCountArray = cellValue3.toUtf8();

        if (hwidCountArray.size() != 2)
        {
           QMessageBox::warning(this,"Input Error","Please enter a valid number for HWID." );
        }
        else
        {
           uniqueDataHandler->content.HWIDCount = hwidCountArray;
           qDebug() << "Cell value: " << hwidCountArray;
        }
        //QByteArray HWID = uniqueDataHandler->content.HWID;
        //hwid için size belirle.
        int HWIDTimes = cellValue3.toInt(nullptr, 16);
        for (int idx = 0; idx < HWIDTimes; ++idx)
        {
            QByteArray HWID = uniqueDataHandler->content.HWID;
            qDebug() << "Entered HWID: " << HWID.toInt(nullptr, 16);
        }
     }
}



void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    QString cellValue = ui->tableWidget->item(row,column)->text();
}

void MainWindow::populateTable()
{
    QDateTime date = uniqueDataHandler->content.DateOfExpiry;

    quint64 epochTime = date.toSecsSinceEpoch();
    QString dateString = QString::number(epochTime);

    quint32 hwid = uniqueDataHandler->content.TokenHWID.toHex().toUInt(nullptr, 16);
    QString hwidString = QString::number(hwid, 16);
    qDebug() << "Generated token: " << hwidString;

    int rowHWID = 1;
    int row = 3;
    int column = 0;


    if (ui->tableWidget->rowCount() <= row) {
        ui->tableWidget->setRowCount(row + 1);
    }
    if (ui->tableWidget->columnCount() <= column) {
        ui->tableWidget->setColumnCount(column + 1);
    }

    QTableWidgetItem *newItem = new QTableWidgetItem(dateString);
    ui->tableWidget->setItem(row, column, newItem);

    QTableWidgetItem *hwidItem = new QTableWidgetItem(hwidString);
    ui->tableWidget->setItem(rowHWID, column, hwidItem);

}

void MainWindow::setColumnWidths()
{
    ui->tableWidget->resizeColumnsToContents();
}

//aes_key.hex dosyasında yazanları fromhex ile beraber 32 bytelık bi stringe dönüştürüp dosyaya yazdıktan sonra işlem yap.
void MainWindow::keyGenerator()
{
    const int key_length = 32;
    unsigned char key[key_length];

    // Generate a random key
    if (!RAND_bytes(key, key_length))
    {
        qCritical() << "Error generating random IV";
        return;
    }

    qDebug() << "Rand Key: " << QByteArray(reinterpret_cast<const char*>(key), key_length);
    qDebug() << "Rand Key (Hex): " << QByteArray(reinterpret_cast<const char*>(key), key_length).toHex();

    QFile file("aes_key.hex");

    if(!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not open file for writing:" << file.errorString();
        return;
    }

    qint64 hexWritten = file.write(QByteArray(reinterpret_cast<const char*>(key), key_length).toHex());
    qDebug() << "Written: " << hexWritten << "/" << QByteArray(reinterpret_cast<const char*>(key), key_length).size();



    /*

    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Could not open file for reading:" << file.errorString();
        return;
    }

    // Read the contents of the file into a QByteArray
    QByteArray fileData = file.readAll();

    // Output the contents of the file
    qDebug() << "File contents1:" << fileData.toHex();

    QString key_toHex = fileData.toHex();
    QString key_fromHex = QByteArray::fromHex(key_toHex);

    file.close();

    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "Could not open file for writing:" << file.errorString();
        return;
    }
    */

}

void MainWindow::IV()
{
    qDebug() << "Current working directory:" << QDir::currentPath();
    const int iv_length = 16;
    unsigned char iv[iv_length];

    // Serialize the relevant data from uniqueDataHandler
    QByteArray serializedData;

    QByteArray idHex = uniqueDataHandler->content.cellValueArray;
    serializedData.append(idHex);
    qDebug() << "id: " << serializedData;

    quint32 hwid = uniqueDataHandler->content.TokenHWID.toHex().toUInt(nullptr, 16);
    QString hwidString = QString::number(hwid, 16);
    qDebug() << "Generated Token: " << hwidString;

    std::string abcd = hwidString.toStdString();
    serializedData.append(abcd.data());
    qDebug() << "token: " << serializedData;

    serializedData.append(uniqueDataHandler->content.HWIDCount);
    qDebug() << "hwidCount: " << serializedData;

    // Convert DateOfExpiry to a byte array
    qint64 epochTime = uniqueDataHandler->content.DateOfExpiry.toSecsSinceEpoch();
    QByteArray dateBytes = QByteArray::number(epochTime);
    serializedData.append(dateBytes);
    qDebug() << "date: " << serializedData;

    serializedData.append(idHex);
    qDebug() << "id: " << serializedData;

    serializedData.append(abcd.data());
    qDebug() << "token: " << serializedData;

    // Concatenate the IV with the serialized data
    QByteArray ivWithSerializedData(reinterpret_cast<const char*>(iv), iv_length);
    ivWithSerializedData.append(serializedData);

    qDebug() << "IV: " << serializedData;

}

void MainWindow::HWIDArray()
{
    uniqueDataHandler->content.HWIDCount;

}

//for setting the scroll bar
//	ui->tableView->setVerticalScrollBar(new QScrollBar(ui->tableView));

