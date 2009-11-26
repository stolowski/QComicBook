#include "ArchiverStatus.h"

using namespace QComicBook;

ArchiverStatus::ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QString &exec1, const QString &exec2)
    : m_name(name),
      m_supported(supported),
      m_extensions(extensions)
{
    m_executables.append(exec1);
    if (exec2 != QString::null)
    {
        m_executables.append(exec2);
    }
}

ArchiverStatus::ArchiverStatus(bool supported, const QString &name, const QStringList &extensions, const QStringList &execlist)
    : m_name(name),
      m_supported(supported),
      m_executables(execlist),
      m_extensions(extensions)
{
}

ArchiverStatus::~ArchiverStatus()
{
}

bool ArchiverStatus::isSupported() const
{
    return m_supported;
}

QString ArchiverStatus::name() const
{
    return m_name;
}

QStringList ArchiverStatus::executables() const
{
    return m_executables;
}

QStringList ArchiverStatus::extensions() const
{
    return m_extensions;
}
