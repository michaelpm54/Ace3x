/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/plaintext-viewer.hpp"

#include "vfs/vfs-entry.hpp"

namespace Ui {
class PlaintextViewer;
}

PlaintextViewer::PlaintextViewer(QWidget *parent)
    : Viewer(parent)
    , ui_(new Ui::PlaintextViewer())
{
    ui_->setupUi(this);
}

void PlaintextViewer::activate(const VfsEntry *item)
{
    show();
    QString text = QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(item->data), item->size));
    ui_->textBrowser->setText(text);
}

bool PlaintextViewer::shouldBeEnabled(const VfsEntry *) const
{
    return true;
}
