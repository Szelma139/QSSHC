#ifndef TABDIALOG_H
#define TABDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>

class TabDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TabDialog(QString filename, QWidget widget, QDialog *parent = nullptr);

signals:

private:
    QTabWidget * tabWidget;
    QDialogButtonBox * buttonBox;

public slots:
};

#endif // TABDIALOG_H
