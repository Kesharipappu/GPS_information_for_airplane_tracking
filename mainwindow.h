#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

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
    void fetchData();
    void handleNetworkData(QNetworkReply *reply);
    void loadFromFile();
    void saveToFile(const QByteArray &data);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QTimer *timer;

    void populateTable(const QJsonArray &statesArray);
};

#endif // MAINWINDOW_H
