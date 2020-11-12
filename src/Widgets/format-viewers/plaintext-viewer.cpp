/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/plaintext-viewer.hpp"

#include <QByteArray>

#include "tree-entries/tree-entry.hpp"

PlaintextViewer::PlaintextViewer(QWidget *parent)
    : QWidget(parent)
{
    ui_.setupUi(this);
    setWindowTitle("Ace3x - Plaintext Viewer");
    resize(800, 600);
}

void PlaintextViewer::activate(const TreeEntry *item)
{
    show();
    QString text = QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(item->getData()), item->getSize()));
    ui_.textBrowser->setText(text);
}

bool PlaintextViewer::shouldBeEnabled(const TreeEntry *) const
{
    return true;
}
