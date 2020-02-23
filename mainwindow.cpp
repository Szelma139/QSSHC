#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <qsshsocket.h>
#include <QGridLayout>
#include <iostream>
//#include <form.h>
#include <QDesktopServices>
#include <QSettings>
#include <iostream>
#include <mywidget.h>
#include <QScrollArea>
#include <QScrollBar>
#include <QDateTime>
#include <mainsettings.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mainSettings::setLocalPath("/home/radek/Wideo/");
    //mainSettings::setRemotePath("/home/radek/Remote/");
    mainSettings::setRemotePath("/opt/cfg/");

    mainSettings::setUser("root");


    ui->setupUi(this);
    //socket = new QSshSocket(this);
    s = new QSshSocket(this);
    connect(s,SIGNAL(downloadProgress(qint64,qint64)),this, SLOT(downloadProgress(qint64,qint64)));
    connect(s,SIGNAL(downloadFinished(int)),this,SLOT(downloadFinished(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}




int MainWindow::getNumberOfFiles()
{
    return list.size();
}

QList<QString> MainWindow::getListOfFiles()
{

}

QString MainWindow::getElementAtIndex(int i)
{
    //qDebug() << list.at(i);
    return list.at(i);
}


void MainWindow::readFile()
{

    list=listOfFiles;
    for (int i=0; i<list.size();i++)
    {
        ui->log->addItem("Plik" + list.at(i) + "dodany do kolejki pobierania");

    }
    //qDebug() << list;
}


void MainWindow::on_downloadpushButton_clicked()
{
    ip = ui->iplineEdit->text();
    port = ui->portlineEdit->text();
    s->connectToHost(ip, port.toInt());
sleepFor(100);
    s->login(mainSettings::getUser(),mainSettings::getPassword());
//analogicznie na srg find /opt/cfg/ -maxdepth 1 -type f -follow -print

    //QString command ="find " +mainSettings::getRemotePath() + " -maxdepth 1 -type f -printf \"%f\n\"";
   QString command = "find " + mainSettings::getRemotePath() + " -maxdepth 1 -type f -follow -print";
    s->executeCommand(command);
sleepFor(1000);
    list = s->getCommandResult();
    QDir d;
    qDebug() << "application path" << d.absolutePath() + "/DownloadedFiles/";
      //  readFile(); //czytanie z listy
    if (!QDir(d.absolutePath() + "/DownloadedFiles").exists())
    {
        ui->log->addItem("Folder docelowy nie istnieje. Proba stworzenia");
        QDir().mkdir("DownloadedFiles");

    }
    ui->log->addItem("Folder docelowy istnieje. Próba połączenia");


    if (getNumberOfFiles() <=0) return;


s->connectToHost(ip, port.toInt());

    for (int i = 0; i<getNumberOfFiles();i++)
    {

        s->connectToHost(ip, port.toInt());

        s->login(mainSettings::getUser(), mainSettings::getPassword());
        //sleepFor(400);
     

//if (s->getLoginSuccess() == 1) {ui->log->addItem("Pomyslne logowanie");}
//else { ui->log->addItem("Blad logowania");
  //  return;}


            QDir dir;
            QString absoluteLocalPath = dir.absolutePath() + "/DownloadedFiles/";
            //QString filename = getElementAtIndex(i);// + ".ini";
         QString filename = list.at(i);
            // QString remotePath = "/home/radek/Remote/";
             QString remotePath = mainSettings::getRemotePath();
            //QString remotePath = "/opt/cfg/";
            //QString remotePath = "/opt/";

            // QString targetDirectory = dir.absoluteFilePath(filename);

            s->pullFile(absoluteLocalPath + filename , remotePath + filename);

            // s->pullFile(targetDirectory + "/DownloadedFiles/" , "/home/radek/Remote/" + filename);
            // QString number=QString::number(count);
            // s->pullFile("/home/radek/Wideo/" + number + ".ini", "/home/radek/Remote/"  + number + ".ini");
            //QString download_directory = QStandardPaths.locate

            // s->pullFile(d.absolutePath() + "/DownloadedFiles/ " + getElementAtIndex(i) + ".ini", "/home/radek/Remote/" + getElementAtIndex(i) + ".ini");

            ui->log->addItem("Pobieranie pliku " + filename + " ze zdalnej lokalizacji" + remotePath  + filename );

            //sciezka na lokalnej maszynie z dokladna nazwa pliku
            //sciezka na maszynie z ktorej sa poberane pliki

    }

    //ui->log->addItem("Pomyslnie ukonczono pobieranie plikow");

}

void MainWindow::on_commandLinkButton_clicked(){

    /// Alloc widgets on the heap
    //
    //
    //
    QVector<QVector<QString>> vec;
    vec.push_back(volumeDictionary);
    vec.push_back(loadDictionary);

    int k = list.length();
    MyWidget * widget[k]; //number of widgets for QTab
    //f->show();
    tabWidget = new QTabWidget();
    QVBoxLayout * layout = new QVBoxLayout();
    //QScrollArea * scrollArea = new QScrollArea;
    //scrollArea->setBackgroundRole(QPalette::Dark);

    //layout->setSizeConstraint(QLayout::SetNoConstraint);

    //-----------------------------------
    ////// read tags from each file from list
    for (int p = 0; p<k; p++)
    {
        readTags(list[p]);// + ".ini");

        int t = hash.size();
        qDebug() << " Hash size " << t;

        widget[p] = new MyWidget(hash, list[p], ip, port,vec.at(p), this);
        layout->addWidget(widget[p]);
        tabWidget->addTab(widget[p],list[p]);

    }
    //   setLayout(layout);
    //  setMinimumSize(200, 200);

    // Form  * f = new Form(tabWidget,this);
    //f->showFullScreen();
    QScrollArea * scrollArea = new QScrollArea;
    scrollArea->setWidget(tabWidget);
    scrollArea->show();
    //tabWidget->showMaximized();

}
QList<QString> MainWindow::getList() const
{
    return list;
}

void MainWindow::showErrorConnection()
{

}
void MainWindow::showErrorPasswordIp()
{

}

void MainWindow::setStatusLabel(QString status)
{
    ui->STATUS->setText(status);
}

void MainWindow::readTags(QString filename)
{
    ////clearing arrays
    //
    //

    Keys.clear();
    values.clear();
    hash.clear();
    //////////

    /////
    //
    // read file
    //
    QSettings settings(QDir::currentPath() + "/DownloadedFiles/" + filename, QSettings::IniFormat);
    Q_FOREACH (QString group, settings.childGroups()) {
        settings.beginGroup(group);
        const QStringList childKeys = settings.childKeys();
        foreach (const QString &childKey, childKeys)
        {

            Keys << childKey;
            values << settings.value(childKey).toString();
        }

        settings.endGroup();
    }

    //------------------------------------------
    //def key value
    //  insertToVector
    //
    int n = Keys.length();
    for (int i=0; i<n; i++)
    {
        Keymap t;
        t.setKey(Keys.at(i));
        t.setValue(values.at(i));
        hash.push_back(t);
        //hash.push_back(new Keymap(Keys.at(i),values.at(i));
    }
    //filling vector with Keymaps
    //---------------------------------------------

}


void MainWindow::setList(const QStringList &value)
{
    list = value;
}

void MainWindow::downloadProgress(qint64 recieved, qint64 total)
{
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(recieved/100);

}



void MainWindow::downloadFinished(int numOfFiles)
{
    ui->progressBar->setValue(100);
    QString labelText = "Pobieranie pliku nr -  " + QString::number(currentNumberOfFile) + " / " + QString::number(numOfFiles);
    ui->numberOfFiles->setText(labelText);
    currentNumberOfFile++;
}

void MainWindow::on_pushButton_clicked()
{
    ui->log->clear();
}

void MainWindow::on_pushButton_2_clicked()
{
    QDesktopServices s;
    //if default
    s.openUrl(QCoreApplication::applicationDirPath());


}

void MainWindow::on_actionWyj_cie_triggered()
{
    QApplication::quit();
}

void MainWindow::sleepFor(qint64 milliseconds){
    qint64 timeToExitFunction = QDateTime::currentMSecsSinceEpoch()+milliseconds;
    while(timeToExitFunction>QDateTime::currentMSecsSinceEpoch()){
        QApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void MainWindow::on_pushButton_3_clicked()
{

    if(s->getLoginSuccess()==1)
    {
    QString command = ui->lineEdit->text();
    s->executeCommand(command);
    sleepFor(400);
    qDebug() << s->getCommandResult(); //stringlista
}
    else
        ui->log->addItem("Nie polaczono. Wcisnij przycisk pobierz, aby polaczyc sie z serwerem");

}


