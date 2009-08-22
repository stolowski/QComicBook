#ifndef __ARCHIVER_STRATEGY
#define __ARCHIVER_STRATEGY

#include <QString>
#include <QStringList>
#include "FileSignature.h"

class QFile;

namespace QComicBook
{
    class ArchiverStrategy
    {
    public:
        ArchiverStrategy(const QString &name, const FileSignature &sig);
        virtual ~ArchiverStrategy();

        virtual bool canOpen(QFile *f) const;
        virtual bool canOpen(const QString &filename) const;
        QStringList getExtractArguments(const QString &filename) const;
        QStringList getListArguments(const QString &filename) const;
        QStringList getExtensions() const;
        virtual void configure() = 0;

    protected:
        static QStringList getArguments(const QStringList & inargs, const QString &filename);
        void setFileMagic(const FileSignature &sig);
        void setExtractArguments(const QString &command);
        void setListArguments(const QString &command);
        void setSupported(bool f=true);
        void addExtension(const QString &ext);

    private:
        FileSignature signature;
        QString name;
        bool supported;
        QStringList extensions;
        QStringList extractArgs;
        QStringList listArgs;
    };
}

#endif
