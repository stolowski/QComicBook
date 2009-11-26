#include "SystemInfoDialog.h"
#include "ArchiversConfiguration.h"
#include "ImageFormatsInfo.h"

using namespace QComicBook;

SystemInfoDialog::SystemInfoDialog(QWidget *parent): QDialog(parent)
{
    setupUi(this);
    updateInfoText();
}

void SystemInfoDialog::updateInfoText()
{
    QString text;
    
    QList<ArchiverStatus> supported, unsupported;

    foreach (ArchiverStatus s, ArchiversConfiguration::instance().getArchiversStatus())
    {
        if (s.isSupported())
        {
            supported.append(s);
        }
        else
        {
            unsupported.append(s);
        }
    }

    if (supported.size())
    {
        text.append("<p>");
        text.append("<h2>");
        text.append(tr("Supported archivers"));
        text.append("</h2>");
    
        text.append("<table border=1 cellspacing=0 cellpadding=5 width=100%>");
        text.append("<tr>");
        text.append("<th width=30%>");
        text.append(tr("Archive type"));
        text.append("</th>");
        text.append("<th>");
        text.append(tr("Supported file extensions"));
        text.append("</th>");
        text.append("</tr>");
        foreach (ArchiverStatus s, supported)
        {
            text.append("<tr>");
            text.append("<td>");
            text.append(s.name());
            text.append("</td>");
            text.append("<td>");
            text.append(s.extensions().join(" "));
            text.append("</td>");
            text.append("</tr>");
        }
        text.append("</table>");
        text.append("</p>");
    }

    if (unsupported.size())
    {
        text.append("<p>");
        text.append("<h2>");
        text.append(tr("Unsupported archivers"));
        text.append("</h2>");

        text.append("<table border=1 cellspacing=0 cellpadding=5 width=100%>");
        text.append("<tr>");
        text.append("<th width=30%>");
        text.append(tr("Archive type"));
        text.append("</th>");
        text.append("<th>");
        text.append(tr("Required executable(s)"));
        text.append("</th>");
        text.append("</tr>");
        foreach (ArchiverStatus s, unsupported)
        {
            text.append("<tr>");
            text.append("<td>");
            text.append(s.name());
            text.append("</td>");
            text.append("<td>");
            text.append(s.executables().join(", "));
            text.append("</td>");
            text.append("</tr>");
        }
        text.append("</table>");
        text.append("</p>");

        text.append("<p>");
        text.append("<h3>");
        text.append(tr("Hint"));
        text.append("</h3>");

        text.append(tr("Some archives are not supported because essential external utilities are missing. "
                       "Please install required executables and restart QComicBook. "
                       "Note: if more than one executable is listed for given archive type, then it is sufficient to install just one of them."));
        text.append("</p>");
    }

    text.append("<p>");
    text.append("<h2>");
    text.append(tr("Supported image formats"));
    text.append("</h2>");
    text.append(ImageFormatsInfo::instance().formats().join(", "));
    text.append("</p>");

    te_sysinfo->setHtml(text);
}
