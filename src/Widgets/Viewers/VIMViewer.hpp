/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VIMVIEWER_HPP
#define VIMVIEWER_HPP

#include <QHBoxLayout>
#include <QListWidget>
#include <QWidget>

#include "Viewer.hpp"
#include "formats/VifMesh.hpp"
#include "ui_VIMViewer.h"


class VIMViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit VIMViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    Ui::VIMViewer ui;
    VifMesh m_vim;
    const TreeEntry *m_item;

private slots:
    void sub0Changed();
};

#endif    // VIMVIEWER_HPP
