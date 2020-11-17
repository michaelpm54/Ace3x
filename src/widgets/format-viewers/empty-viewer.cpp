/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/empty-viewer.hpp"

#include "ui_empty-viewer.h"

EmptyViewer::EmptyViewer(QWidget *parent)
    : Viewer(parent)
    , ui_(new Ui_EmptyViewer())
{
    ui_->setupUi(this);
}

void EmptyViewer::activate(const VfsEntry *item)
{
}

bool EmptyViewer::shouldBeEnabled(const VfsEntry *) const
{
    return true;
}
