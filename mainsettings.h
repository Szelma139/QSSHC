#ifndef MAINSETTINGS_H
#define MAINSETTINGS_H

#include <QString>

class mainSettings
{
public:
    mainSettings();

    static QString getLocalPath();
    static void setLocalPath(const QString &value);

    static QString getRemotePath();
    static void setRemotePath(const QString &value);

    static QString getUser();
    static void setUser(const QString &value);

    static QString getPassword();
    static void setPassword(const QString &value);

private:
    static QString localPath;
    static QString remotePath;
    static QString user;
    static QString password;
};


#endif // MAINSETTINGS_H
