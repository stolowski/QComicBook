#ifndef __JOBKEY_H
#define __JOBKEY_H

namespace QComicBook
{
    class JobKey
    {
    public:
        JobKey(int subsystem, int uniqKey): m_subsys(subsystem), m_key(uniqKey) {}
        int getSubsystem() const { return m_subsys; }
        int getKey() const { return m_key; }
   
    private:
        const int m_subsys;
        const int m_key;
    };
}

#endif
