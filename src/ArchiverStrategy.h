#ifndef __ARCHIVER_STRATEGY
#define __ARCHIVER_STRATEGY

#include <QString>
#include <QStringList>
#include "FileSignature.h"
#include "ArchiverStatus.h"

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
        QStringList getExtractArguments() const;
        QStringList getListArguments(const QString &filename) const;
        QStringList getListArguments() const;
        QStringList getExtensions() const;
        static QStringList fillTemplateArguments(const QStringList &args, const QString &filename);
        virtual void configure() = 0;

        operator ArchiverStatus() const;

    protected:
        void setFileMagic(const FileSignature &sig);
        void setExtractArguments(const QString &command);
        void setListArguments(const QString &command);
        void setSupported(bool f=true);
        void setExecutables(const QString &exec1, const QString &exec2=QString::null);
        void addExtension(const QString &ext);

    private:
        FileSignature signature;
        QString name;
        bool supported;
        QStringList executables;
        QStringList extensions;
        QStringList extractArgs;
        QStringList listArgs;
    };
}

#endif
