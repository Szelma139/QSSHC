#include "form.h"
#include "ui_form.h"
#include <QPushButton>
#include <tabdialog.h>


Form::Form(QTabWidget * qTab, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
qTab->showFullScreen();
//ui->tabWidget->


}

Form::~Form()
{
    delete ui;
}
