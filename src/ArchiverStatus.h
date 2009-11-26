#ifndef __ARCHIVER_STATUS_H
#define __ARCHIVER_STATUS_H

#include <QString>
#include <QStringList>

namespace QComicBook
{
    class ArchiverStatus
    {
    public:
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QString &exec1, const QString &exec2=QString::null);
        ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QStringList &execlist);
        ~ArchiverStatus();

        bool isSupported() const;
        QString name() const;
        QStringList executables() const;
        QStringList extensions() const;

    private:
        QString m_name;
        QStringList m_executables;
        QStringList m_extensions;
        bool m_supported;
    };
}

#endif
