/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef PLAINTEXTVIEWER_HPP
#define PLAINTEXTVIEWER_HPP

#include <QWidget>

#include "Viewer.hpp"
#include "ui_PlaintextViewer.h"

class TreeEntry;

class PlaintextViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit PlaintextViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    Ui::PlaintextViewer ui;
};

#endif    // PLAINTEXTVIEWER_HPP
