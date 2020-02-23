#include "qsshsocket.h"
#include <QFileInfo>
#include <iostream>
#include <QTextCodec>
// if compiling in windows, add needed flags.
#ifdef _WIN32
#   include <io.h>

typedef int mode_t;

/// @Note If STRICT_UGO_PERMISSIONS is not defined, then setting Read for any
///       of User, Group, or Other will set Read for User and setting Write
///       will set Write for User.  Otherwise, Read and Write for Group and
///       Other are ignored.
///
/// @Note For the POSIX modes that do not have a Windows equivalent, the modes
///       defined here use the POSIX values left shifted 16 bits.

static const mode_t S_ISUID      = 0x08000000;           ///< does nothing
static const mode_t S_ISGID      = 0x04000000;           ///< does nothing
static const mode_t S_ISVTX      = 0x02000000;           ///< does nothing
static const mode_t S_IRUSR      = mode_t(_S_IREAD);     ///< read by user
static const mode_t S_IWUSR      = mode_t(_S_IWRITE);    ///< write by user
static const mode_t S_IXUSR      = 0x00400000;           ///< does nothing
#   ifndef STRICT_UGO_PERMISSIONS
static const mode_t S_IRGRP      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWGRP      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXGRP      = 0x00080000;           ///< does nothing
static const mode_t S_IROTH      = mode_t(_S_IREAD);     ///< read by *USER*
static const mode_t S_IWOTH      = mode_t(_S_IWRITE);    ///< write by *USER*
static const mode_t S_IXOTH      = 0x00010000;           ///< does nothing
#   else
static const mode_t S_IRGRP      = 0x00200000;           ///< does nothing
static const mode_t S_IWGRP      = 0x00100000;           ///< does nothing
static const mode_t S_IXGRP      = 0x00080000;           ///< does nothing
static const mode_t S_IROTH      = 0x00040000;           ///< does nothing
static const mode_t S_IWOTH      = 0x00020000;           ///< does nothing
static const mode_t S_IXOTH      = 0x00010000;           ///< does nothing
#   endif
static const mode_t MS_MODE_MASK = 0x0000ffff;           ///< low word
#endif


QSshSocket::QSshSocket(QObject * parent)
    :QThread(parent)
{

    m_host = "";
    m_user = "";
    m_password = "";
    // m_port = -1;
    m_port = 22;
    m_loggedIn = false;
    m_session  = NULL;
    m_workingDirectory = ".";

    qRegisterMetaType<QSshSocket::SshError>("QSshSocket::SshError");
    m_currentOperation.executed = true;
   connect(this,SIGNAL(currentStatus(QString)),parent,SLOT(setStatusLabel(QString)));

    m_run = true;
    //connect(this,SIGNAL(downloadProgress(qint64,qint64)), parent,SIGNAL(downloadProgress(qint64,qint64)));
    //  connect(this,SIGNAL(downloadFinished(int)),parent,SIGNAL(downloadFinished(int)));
    start();
    emit currentStatus("Wątek rozpoczął pracę.");
}

QSshSocket::~QSshSocket()
{
    m_run = false;
    this->wait();
}




void QSshSocket::run()
{

    while(m_run) //jesli odpalno
    {
        if (m_session == NULL)
        {

            if (m_host != "")
            {
                emit currentStatus("Tworzenie nowej sesji SSH.");
                m_session = ssh_new();

                //set logging to verbose so all errors can be debugged if crash happens
                int verbosity = SSH_LOG_PROTOCOL;

                // set the pertinant ssh session options
                ssh_options_set(m_session, SSH_OPTIONS_HOST, m_host.toAscii().data());
                ssh_options_set(m_session, SSH_OPTIONS_USER, m_user.toAscii().data());
                ssh_options_set(m_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
                ssh_options_set(m_session, SSH_OPTIONS_PORT, &m_port);

                // try to connect given host, user, port
                int connectionResponse = ssh_connect(m_session);

                // if connection is Successful keep track of connection info.
                if (connectionResponse == SSH_OK)
                {
                    m_connectionSuccess = 1;
                    connected();
                    emit currentStatus("Połączono");

                }
                else
                {
                    m_connectionSuccess = 2;
                    error(SessionCreationError);
                    emit currentStatus("Błąd połączenia");
                    return;
                    m_session = NULL;
                }
            }

        }


        // if we have a vaild ssh connection, authenticate connection with credentials
        else if (!m_loggedIn)
        {


            // check to see if a username and a password have been given
            if (m_user != "" && m_password !="")
            {

                // try authenticating current user at remote host
                int worked = ssh_userauth_password(m_session, m_user.toAscii().data(), m_password.toAscii().data());


                // if successful, store user password.
                if (worked == SSH_OK)
                {
                    loginSuccessful();
                    m_loggedIn = true;
                    m_loginSuccess = 1;
                    emit currentStatus ("Zalogowano");
                }
                else
                {
                    m_user = "";
                    m_password = "";
                    m_loginSuccess = 2;
                    error(PasswordAuthenticationFailedError);
                    emit currentStatus ("Błąd logowania");
                    return;

                }


            }
        }
        // if all ssh setup has been completed, check to see if we have any commands to execute
        else if (!m_currentOperation.executed)
        {


            if (m_currentOperation.type == Command || m_currentOperation.type == WorkingDirectoryTest)
            {

                // attempt to open ssh shell channel
                ssh_channel channel = ssh_channel_new(m_session);

                // if attempt fails,return
                if (ssh_channel_open_session(channel) != SSH_OK)
                {
                    error(ChannelCreationError);
                                emit currentStatus("Nieporawny kanal/sesja SSH");
                }


                int requestResponse = SSH_AGAIN;

                // attempt to execute shell command
                while (requestResponse == SSH_AGAIN)
                    requestResponse = ssh_channel_request_exec(channel, m_currentOperation.adminCommand.toAscii().data());

                // if attempt not executed, close connection then return
                if (requestResponse != SSH_OK)
                {
                    emit currentStatus("Brak odpowiedzi o serwera");
                    error(ChannelCreationError);
                }


                QByteArray buffer;
                buffer.resize(2000);
                // read in command result

                emit currentStatus("Pobieranie pliku " + buffer.size());
                int totalBytes = 0, newBytes = 0;
                do
                {
                    //pasek ladowania
                    //ochrona przed brakiem pliku
                    newBytes = ssh_channel_read(channel, &(buffer.data()[totalBytes]), buffer.size() - totalBytes, 0);
                    if (newBytes > 0)
                        totalBytes += newBytes;

                    int i;
                    char *enable = "stic-isr2951-t1";
                    int ret;
                    //totalBytes += newBytes;
                    //fprintf(stderr, "We read [%d] bytes:\n", totalBytes);
                    for( i=0; i < newBytes; ++i )
                        fputc( buffer[i], stderr);
                    ret = ssh_channel_write(channel, enable, strlen(enable));

                    //   fprintf(stderr, "Wrote [%d] bytes\n", ret);
                    //  QString DataAsString = QTextCodec::codecForMib(1015)->toUnicode(buffer[i]);
                    //qDebug()<< "Data AS String - " << DataAsString;
                }while (newBytes > 0);

                QString resultOfCommand = QString(buffer);
                QStringList list = resultOfCommand.split('\n');
                QStringList onlyNamesList;
                for (auto l : list)
                {
                    if (l=="")

                        list.removeOne(l);
                   QString temp = l;
                   temp = temp.right(temp.length()-9);
                   l=temp;

                   if (l!="")
                   onlyNamesList.push_back(l);

                    // qDebug() << l;
                }
                // qDebug() << "komenda - "<<  list.last();
                //qDebug() << "komenda - "<<  list.first();
                //qDebug() << "Vec " << vec.back();
                // close channel
                //commandResult = list;
                commandResult = onlyNamesList;
                // getElementsOfList(commandResult);
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
                ssh_channel_free(channel);

                QString response = QString(buffer).mid(0,totalBytes);
                response.replace("\n","");
                if (m_currentOperation.type == WorkingDirectoryTest)
                {
                    if (response == "exists")
                        m_workingDirectory = m_nextWorkingDir;
                    m_nextWorkingDir = ".";
                    workingDirectorySet(m_workingDirectory);
                }
                else
                    commandExecuted( m_currentOperation.command, response) ;

            }
            // if all ssh setup has been completed, check to see if we have any file transfers to execute
            else if (m_currentOperation.type == Pull)
            {
                ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_READ, m_currentOperation.remotePath.toAscii().data());
                if (scpSession == NULL)
                    error(ScpChannelCreationError);

                // attempt to initialize new scp session.
                int scpInitialized = ssh_scp_init(scpSession);
                if(scpInitialized != SSH_OK)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpChannelCreationError);
                   // return;
                }


                // attempt to authorize new scp pull
                if (ssh_scp_pull_request(scpSession) != SSH_SCP_REQUEST_NEWFILE)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpPullRequestError);
                }

                // accept authorization
                ssh_scp_accept_request(scpSession);


                // get remote file size
                int size = ssh_scp_request_get_size(scpSession);

                // resize buffer, read remote file into buffer
                QByteArray buffer;
                buffer.resize(size);

                // if an error happens while reading, close the scp session and return
                if (ssh_scp_read(scpSession, buffer.data() , size) == SSH_ERROR)
                {https://downloads.avaya.com/elmodocs2/ip_phones/r2_7/output/16_601433_2/IPTel_Server044.html
                        ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpReadError);
                                emit currentStatus("Błąd SCP");
                    fileResultInfo = "Blad SCP";
                  // return;
                }

                // loop until eof flag
                if  (ssh_scp_pull_request(scpSession)  != SSH_SCP_REQUEST_EOF)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpReadError);
                    emit currentStatus("Błąd EOF podczas pobierania");
                    fileResultInfo = " Blad End Of File";


                }
                //emit downloadProgress(quint64(buffer.size())+quint64(buffer.size()),quint64(size));
                //close scp session
                ssh_scp_close(scpSession);
                ssh_scp_free(scpSession);

                // open up local file and write contents of buffer to it.
                QFile file(m_currentOperation.localPath);
                file.open(QIODevice::WriteOnly);
                file.write(buffer);

                fileResultInfo = " Zapisywanie pliku " + buffer.size();
                emit currentStatus("Zapisywanie pliku na dysk");

                //emit downloadProgress(buffer.size(),file.size());

                file.close();
                emit downloadFinished(numOfFiles);
                numOfFiles ++;
                pullSuccessful(m_currentOperation.localPath,m_currentOperation.remotePath);


            }
            else if (m_currentOperation.type == Push)
            {
                            emit currentStatus( "Wysylanie danych na serwer.");
                // attempt to create new scp from ssh session.
                ssh_scp scpSession = ssh_scp_new(m_session,SSH_SCP_WRITE, m_currentOperation.remotePath.toAscii().data());

                // if creation failed, return
                if (scpSession == NULL)
                {
                       emit currentStatus("Socket Error.");
                    error(SocketError);
                    break;

                }

                // attempt to initialize new scp session.
                int scpInitialized = ssh_scp_init(scpSession);


                // if failed, close scp session and return.
                if(scpInitialized != SSH_OK)
                {
                    emit currentStatus("Blad inicjalziacji sesji SSH.");
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpChannelCreationError);
                    //break;
                }


                // open the local file and check to make sure it exists
                // if not, close scp session and return.
                QFile file(m_currentOperation.localPath);
                if (!file.exists())
                {
                       emit currentStatus("Plik z lokalizacji" + m_currentOperation.localPath);
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpFileNotCreatedError);
                    //return;
                }

                // if the file does exist, read all contents as bytes
                file.open(QIODevice::ReadOnly);
                QByteArray buffer = file.readAll();
                         emit currentStatus("Pobieranie pliku o rozmiarze " + buffer.size());
                file.close();

                // attempt to authorize pushing bytes over scp socket
                // if this fails, close scp session and return.
                if (ssh_scp_push_file(scpSession, m_currentOperation.remotePath.toAscii().data(), buffer.size(), S_IRUSR | S_IWUSR) != SSH_OK)
                {
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpPushRequestError);
                   // break;
                }


                // once authorized to push bytes over scp socket, start writing
                // if an error is returned,  close scp session and return.
                if ( ssh_scp_write(scpSession,buffer.data(), buffer.size()) != SSH_OK)
                {
                    emit currentStatus("Blad podczas zapisywania pliku ");
                    ssh_scp_close(scpSession);
                    ssh_scp_free(scpSession);
                    error(ScpWriteError);
                    //break;
                }


                // close scp session and return.
                ssh_scp_close(scpSession);
                ssh_scp_free(scpSession);
          emit currentStatus("Zwalnianie sesji SSH");
                                  emit currentStatus( "Zakonczono ");
                pushSuccessful(m_currentOperation.localPath,m_currentOperation.remotePath);

            }


            m_currentOperation.executed = true;


        }
        else
        {
            msleep(100);
        }

    }

}

QString QSshSocket::getFileResultInfo() const
{
    return fileResultInfo;
}

void QSshSocket::setFileResultInfo(const QString &value)
{
    fileResultInfo = value;
}


int QSshSocket::getLoginSuccess()
{
    return m_loginSuccess;
}

QStringList QSshSocket::getCommandResult()
{
    return commandResult;
}

QString QSshSocket::getElementsOfList(QStringList list)
{
    for (auto l: list)
        qDebug() << l;
}

void QSshSocket::setLoginSuccess(int loginSuccess)
{
    m_loginSuccess = loginSuccess;
}

int QSshSocket::getConnectionSuccess()
{
    return m_connectionSuccess;
}

void QSshSocket::setConnectionSuccess(int value)
{
    m_connectionSuccess = value;
}


bool QSshSocket::returnLoggedIn()
{
    return m_loggedIn;
}

bool QSshSocket::getSessionOpen()
{
    return m_session;
}


void QSshSocket::setOperationNotExecuted(bool value)
{
    m_currentOperation.executed = value;
}
void QSshSocket::disconnectFromHost()
{
    m_host = "";
    m_user = "";
    m_password = "";
    m_port = -1;
    m_loggedIn = false;
    if (m_session != NULL)
    {
        ssh_disconnect(m_session);
        ssh_free(m_session);
    }
    m_session = NULL;
}

void QSshSocket::connectToHost(QString host, int port)
{
    m_host = host;
    m_port = port;
}
void QSshSocket::login(QString user, QString password)
{
    m_user = user;
    m_password = password;
}
void QSshSocket::executeCommand(QString command)
{
    m_currentOperation.type = Command;
    if (m_workingDirectory != ".")
        m_currentOperation.adminCommand = "cd " + m_workingDirectory + "; "  + command;
    else
        m_currentOperation.adminCommand = command ;

    m_currentOperation.command =command;
    m_currentOperation.executed = false;
}

void QSshSocket::pullFile(QString localPath, QString remotePath)
{
    m_currentOperation.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        m_currentOperation.remotePath = remotePath;
    else
        m_currentOperation.remotePath = m_workingDirectory + "/" + remotePath;
    m_currentOperation.type = Pull;
    m_currentOperation.executed = false;
}


void QSshSocket::pushFile(QString localPath, QString remotePath)
{
    m_currentOperation.localPath = localPath;
    if (QFileInfo(remotePath).isAbsolute())
        m_currentOperation.remotePath = remotePath;
    else
        m_currentOperation.remotePath = m_workingDirectory + "/" + remotePath;
    m_currentOperation.type = Push;
    m_currentOperation.executed = false;
}

void QSshSocket::setWorkingDirectory(QString path)
{
    m_nextWorkingDir = path;
    m_currentOperation.type = WorkingDirectoryTest;
    m_currentOperation.adminCommand = "[ -d " + m_nextWorkingDir +" ] && echo 'exists'";
    m_currentOperation.executed = false;
}

bool QSshSocket::isConnected()
{
    return m_session != NULL;
}

bool QSshSocket::isLoggedIn()
{
    return m_loggedIn;
}

QString QSshSocket::user(){return m_user;}
QString QSshSocket::host(){return m_host;}
int QSshSocket::port(){return m_port;}
