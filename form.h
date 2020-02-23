#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QTabWidget * qTab, QWidget *parent = 0);
    ~Form();

private:
    Ui::Form *ui;
    QVBoxLayout * layout;

};

#endif // FORM_H
