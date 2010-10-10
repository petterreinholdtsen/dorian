#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
#include <unistd.h>
#endif

#include <QDir>

#include "platform.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_SYMBIAN)
#   define DORIAN_BASE "dorian"
#else
#   define DORIAN_BASE ".dorian"
#endif

#ifdef Q_WS_MAC
#   define DORIAN_ICON_PREFIX ":/icons/mac/"
#else
#   define DORIAN_ICON_PREFIX ":/icons/"
#endif

QString Platform::dbPath()
{
    QString base(QDir::home().absoluteFilePath(DORIAN_BASE));
    return QDir(base).absoluteFilePath("books.db");
}

QString Platform::icon(const QString &name)
{
    return QString(DORIAN_ICON_PREFIX) + name + ".png";
}

void Platform::restart(char *argv[])
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    extern char **environ;
    execve(argv[0], argv, environ);
#else
    Q_UNUSED(argv);
#endif
}
