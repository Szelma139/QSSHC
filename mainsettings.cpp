#include "mainsettings.h"

mainSettings::mainSettings()
{

}

QString mainSettings::getLocalPath()
{
    return localPath;
}

void mainSettings::setLocalPath(const QString &value)
{
    localPath = value;
}

QString mainSettings::getRemotePath()
{
    return remotePath;
}

void mainSettings::setRemotePath(const QString &value)
{
    remotePath = value;
}

QString mainSettings::getUser()
{
    return user;
}

void mainSettings::setUser(const QString &value)
{
    user = value;
}

QString mainSettings::getPassword()
{
    return password;
}

void mainSettings::setPassword(const QString &value)
{
    password = value;
}

QString mainSettings::remotePath = "";

QString mainSettings::localPath = "";

QString mainSettings::user = "";

QString mainSettings::password = "";
