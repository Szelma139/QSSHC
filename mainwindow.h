#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qsshsocket.h>
#include <tabdialog.h>
#include <keymap.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QList<QString> getListOfFiles();
    QList<QString> getList() const;
    void setList(const QStringList &value);
    int getNumberOfFiles();
    void readFile();
     QString getElementAtIndex(int i);
    void readTags(QString filename);
    void sleepFor(qint64 milliseconds);

private slots:

    void on_downloadpushButton_clicked();
    void on_commandLinkButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_actionWyj_cie_triggered();

    void on_pushButton_3_clicked();


public slots:
    void downloadProgress(qint64 recieved, qint64 total);
    void downloadFinished(int numOfFiles);
    void showErrorConnection();
    void showErrorPasswordIp();
    void setStatusLabel(QString status);

private:
    QList<QString> list;
    Ui::MainWindow *ui;
    int currentNumberOfFile = 1;

    QTabWidget * tabWidget;
    QString globalPath;
    QStringList Keys;
    QStringList values;
    QVector<Keymap> hash;
    QString ip;
    QString port;
    QSshSocket * s;
    QList<QString> listOfFiles = {"volume", "load"};
    QVector<QString> volumeDictionary={"aa","bb","c","d","e","bb","c","bb","c111","bb","c","b%b","c","bb","c","bb","c","bb","c","bb","c","bb","c","bb","c"};
    QVector<QString> loadDictionary={"ab","o","bb","c"};



};

#endif // MAINWINDOW_H
