/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VIMVIEWER_HPP
#define VIMVIEWER_HPP

#include <QHBoxLayout>
#include <QListWidget>
#include <QWidget>

#include "Formats/VifMesh.hpp"
#include "Viewer.hpp"
#include "ui_VIMViewer.h"

class VIMViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit VIMViewer(QWidget *parent = nullptr);

    void activate(const Entry *item) override;
    bool shouldBeEnabled(const Entry *item) const override;

private:
    Ui::VIMViewer ui;
    VifMesh m_vim;
    const Entry *m_item;

private slots:
    void sub0Changed();
};

#endif    // VIMVIEWER_HPP
