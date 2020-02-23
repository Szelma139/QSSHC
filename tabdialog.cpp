#include "tabdialog.h"

TabDialog::TabDialog(QString filename,QWidget widget, QDialog *parent) : QDialog(parent)
{
tabWidget = new QTabWidget;
//tabWidget->addTab(widget,nullptr,filename);
}
