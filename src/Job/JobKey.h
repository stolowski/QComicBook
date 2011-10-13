#ifndef __JOBKEY_H
#define __JOBKEY_H

#include <QDebug>

namespace QComicBook
{
    class JobKey
    {
    public:
        JobKey();
        JobKey(int subsystem, int uniqKey);
        int getSubsystem() const;
        int getKey() const;
   
        bool operator==(const JobKey &other) const;

    private:
        int m_subsys;
        int m_key;
    };
}

QDebug operator<<(QDebug dbg, const QComicBook::JobKey &job);

#endif
