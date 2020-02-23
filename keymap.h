#ifndef KEYMAP_H
#define KEYMAP_H

#include <QString>

class Keymap
{
public:
    Keymap();
    Keymap(QString,QString);

    QString getKey() const;
    void setKey(const QString &value);

    QString getValue() const;
    void setValue(const QString &nvalue);

private:
    QString key;
    QString value;
};

#endif // KEYMAP_H
