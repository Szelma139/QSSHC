#include "keymap.h"

Keymap::Keymap()
{

}

Keymap::Keymap(QString k, QString v)
{
    key = k;
    value = v;
}

QString Keymap::getKey() const
{
    return key;
}

void Keymap::setKey(const QString &value)
{
    key = value;
}

QString Keymap::getValue() const
{
    return value;
}

void Keymap::setValue(const QString &nvalue)
{
    value = nvalue;
}


