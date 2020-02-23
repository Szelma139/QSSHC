#include "mywidget.h"
#include "ui_mywidget.h"
#include <QLabel>
#include <QBoxLayout>
#include <QGridLayout>
#include <QSettings>
#include <qsshsocket.h>
#include <QDebug>
#include <QDir>
#include <QScrollBar>
#include <QScrollArea>
#include <qgroupbox.h>
#include <mainsettings.h>


MyWidget::MyWidget(QVector<Keymap> hash, QString file, QString IP, QString PORT, QVector<QString> dictionary, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyWidget)
{

    ///get values of all line edits and save it back to the vector
    /// get all pointers to some kind of container
    //  ui->setupUi(this);



    valuesFromFile=hash;
    filename = file;
    ip = IP;
    port = PORT;
    numberOfWidgets = valuesFromFile.size();
    labelDictionary = dictionary;

    QLineEdit * lineEdit[numberOfWidgets];
    QLabel * label[numberOfWidgets];
    QGridLayout * layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    //////
    ///
    /// adding widgets to customWidget
    ///
    for (int i=0; i<numberOfWidgets; i++)
    {
        label[i] = new QLabel(this);
        label[i]->setText(valuesFromFile.at(i).getKey());
        label[i]->setText(labelDictionary.at(i));
        layout->addWidget(label[i],i,1,Qt::AlignCenter);
        lineEdit[i]= new QLineEdit;
        lineEdit[i]->setText(valuesFromFile.at(i).getValue());
        QLineContainer.push_back(lineEdit[i]);
        lineEdit[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        layout->addWidget(lineEdit[i],i,2,Qt::AlignCenter);



    }

    sendBackButton = new QPushButton(this);
    saveChangesButton = new QPushButton(this);
    sendBackButton->setText("Wyslij");
    saveChangesButton->setText("Zapisz zmiany");

    layout->addWidget(sendBackButton, numberOfWidgets+1, 1);
    layout->addWidget(saveChangesButton, numberOfWidgets+1, 2);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(layout);
    connect(sendBackButton, SIGNAL(clicked()), this, SLOT(sendBack()));
    connect(saveChangesButton,SIGNAL(clicked()), this, SLOT(saveChanges()));
}

MyWidget::~MyWidget()
{
    delete ui;
}

void MyWidget::sendBack()
{
    s = new QSshSocket(this);
    if (s)
        s->connectToHost(ip, port.toInt());
   
    QString user =  mainSettings::getUser();
    QString password = mainSettings::getPassword();

    s->login(user, password);

    QDir dir;
    QString absoluteLocalPath = dir.absolutePath() + "/DownloadedFiles/";
    //QString remotePath = "/opt/cfg/";

    //QString remotePath = "/home/radek/Remote/";
    QString remotePath = mainSettings::getRemotePath();

    qDebug() << filename;
    s->pushFile(absoluteLocalPath + filename , remotePath + filename);

    // s->deleteLater();
}

void MyWidget::saveChanges()
{
    //
    ////
    ///save changes to qvector
    ///
    ///
    QVector<Keymap> currentValues;

    for (int i = 0; i<numberOfWidgets;i++)
    {
        Keymap t;
        t.setKey(valuesFromFile.at(i).getKey());
        t.setValue(QLineContainer.at(i)->text());
        currentValues.push_back(t);

    }

    for (int p = 0; p<numberOfWidgets; p++)
    {
        if (valuesFromFile.at(p).getValue() != currentValues.at(p).getValue())
        {
            QSettings settings(QDir::currentPath() + "/DownloadedFiles/" + filename, QSettings::IniFormat);
            QString key;
            QString value;

            key = currentValues.at(p).getKey();
            value = currentValues.at(p).getValue();
            qDebug() << settings.childGroups();
            QString group = settings.childGroups().last();
            qDebug() << " Grupa -" << group;
            settings.setValue(group + "/" + key ,value);
       //qlist->addItem(" Ustawiam klucz "  + key + " na wartosc " +
        //             value + " Poprzednia wartosc- "
         //            + valuesFromFile.at(p).getValue());
           // qDebug() << " Ustawiam klucz" << key << "na wartosc"<<
                     //   value << " z wartosci - " <<
                      //  valuesFromFile.at(p).getValue();
        }
    }
    ///

}

void MyWidget::replaceHtmlSpaces()
{
    //QDir dir;
    QFile file (QDir::currentPath() + "/DownloadedFiles/" + filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return;
    }

QString dataText = file.readAll();
QString htmlSpace = "%20";
dataText.replace(htmlSpace, " ");

}
