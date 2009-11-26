#ifndef __SYSTEM_INFO_DIALOG_H
#define __SYSTEM_INFO_DIALOG_H

#include <QDialog>
#include "ui_SystemInfoDialog.h"

namespace QComicBook
{
    class SystemInfoDialog: public QDialog, private Ui::SystemInfoDialog
    {
    public:
        SystemInfoDialog(QWidget *parent);

    private:
        void updateInfoText();
    };
}

#endif
