#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QVector>
#include <keymap.h>
#include <QPushButton>
#include <QLineEdit>
#include <qsshsocket.h>
#include <QListWidget>

namespace Ui {
class MyWidget;
}

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QVector<Keymap>,QString file,QString, QString,QVector<QString>, QWidget *parent = 0);
    ~MyWidget();
    void replaceHtmlSpaces();


private slots:
    void saveChanges();
    void sendBack();

private:
    Ui::MyWidget *ui;
    QVector<QString> labelDictionary; //New values for labels
    QSshSocket * s;
    QVector<Keymap> valuesFromFile;    //Key,Value keymap from file
    QVector<QLineEdit*> QLineContainer; //default values of labels
     QListWidget * qlist ;

    int numberOfWidgets;

    QPushButton * saveChangesButton;
    QPushButton * sendBackButton;

    QString filename;
    QString ip;
    QString port;
};

#endif // MYWIDGET_H
