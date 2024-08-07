#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
    , timer(new QTimer(this))
{
    ui->setupUi(this);
    connect(ui->fetchButton, &QPushButton::clicked, this, &MainWindow::fetchData);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handleNetworkData);

    // Set up tableWidget with column headers
    ui->tableWidget->setColumnCount(17);
    QStringList headers = { "ICAO24 address", "Callsign", "Origin Country", "Time Position",
                           "Last Contact", "Longitude", "Latitude", "Barometric Altitude",
                           "On Ground", "Velocity", "Heading", "Vertical Rate", "Sensors",
                           "Geometric Altitude", "Squawk", "SPI", "Position Source" };
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // Load data from file
    loadFromFile();

    // Set up the timer to fetch data every hour (3600000 milliseconds)
    connect(timer, &QTimer::timeout, this, &MainWindow::fetchData);
    timer->start(3600000);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::fetchData() {
    QUrl url("https://opensky-network.org/api/states/all");
    QNetworkRequest request(url);
    networkManager->get(request);
}

void MainWindow::handleNetworkData(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "Error", reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
    QJsonObject jsonObject = jsonResponse.object();
    QJsonArray statesArray = jsonObject["states"].toArray();

    // Populate the table with the new data
    populateTable(statesArray);

    // Save the data to a file
    saveToFile(response);

    reply->deleteLater();
}

void MainWindow::populateTable(const QJsonArray &statesArray) {
    // Clear the table before inserting new data
    ui->tableWidget->setRowCount(0);

    for (const QJsonValue &value : statesArray) {
        QJsonArray state = value.toArray();
        int currentRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(currentRow);

        ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(state[0].toString()));
        ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(state[1].toString()));
        ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(state[2].toString()));
        ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(QString::number(state[3].toDouble())));
        ui->tableWidget->setItem(currentRow, 4, new QTableWidgetItem(QString::number(state[4].toDouble())));
        ui->tableWidget->setItem(currentRow, 5, new QTableWidgetItem(QString::number(state[5].toDouble())));
        ui->tableWidget->setItem(currentRow, 6, new QTableWidgetItem(QString::number(state[6].toDouble())));
        ui->tableWidget->setItem(currentRow, 7, new QTableWidgetItem(QString::number(state[7].toDouble())));
        ui->tableWidget->setItem(currentRow, 8, new QTableWidgetItem(QString::number(state[8].toDouble())));
        ui->tableWidget->setItem(currentRow, 9, new QTableWidgetItem(QString::number(state[9].toDouble())));
        ui->tableWidget->setItem(currentRow, 10, new QTableWidgetItem(QString::number(state[10].toDouble())));
        ui->tableWidget->setItem(currentRow, 11, new QTableWidgetItem(QString::number(state[11].toDouble())));
        ui->tableWidget->setItem(currentRow, 12, new QTableWidgetItem(QString::number(state[12].toDouble())));
        ui->tableWidget->setItem(currentRow, 13, new QTableWidgetItem(QString::number(state[13].toDouble())));
        ui->tableWidget->setItem(currentRow, 14, new QTableWidgetItem(state[14].toString()));
        ui->tableWidget->setItem(currentRow, 15, new QTableWidgetItem(QString::number(state[15].toDouble())));
        ui->tableWidget->setItem(currentRow, 16, new QTableWidgetItem(QString::number(state[16].toDouble())));
    }
}

void MainWindow::loadFromFile() {
    QFile file("data.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return; // File doesn't exist or couldn't be opened
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = jsonResponse.object();
    QJsonArray statesArray = jsonObject["states"].toArray();

    // Populate the table with the data from the file
    populateTable(statesArray);

    file.close();
}

void MainWindow::saveToFile(const QByteArray &data) {
    QFile file("data.json");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Could not save data to file");
        return;
    }

    file.write(data);
    file.close();
}
