/* SPDX-License-Identifier: GPLv3-or-later */

#include "PlaintextViewer.hpp"

#include <QByteArray>

#include "Entry.hpp"

PlaintextViewer::PlaintextViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Ace3x - Plaintext Viewer");
    resize(800, 600);
}

void PlaintextViewer::activate(const Entry *item)
{
    show();
    QString text = QString::fromLatin1(QByteArray(reinterpret_cast<const char *>(item->getData()), item->getSize()));
    ui.textBrowser->setText(text);
}

bool PlaintextViewer::shouldBeEnabled(const Entry *) const
{
    return true;
}
