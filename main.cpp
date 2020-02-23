#include <QCoreApplication>
#include <mainwindow.h>
#include <QApplication>
#include "qsshsocket.h"
//#include <QQuickStyle>

int main(int argc, char *argv[])
{


    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    QFile styleFile(":/style/style.css");
      if(styleFile.open(QIODevice::ReadOnly))
      {
          QTextStream textStream(&styleFile);
          QString styleSheet = textStream.readAll();
          styleFile.close();
          a.setStyleSheet(styleSheet);
      }
MainWindow m;
m.show();
    return a.exec();
}
