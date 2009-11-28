/*
 * This file is a part of QComicBook.
 *
 * Copyright (C) 2005-2009 Pawel Stolowski <stolowski@gmail.com>
 *
 * QComicBook is free software; you can redestribute it and/or modify it
 * under terms of GNU General Public License by Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY. See GPL for more details.
 */

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
