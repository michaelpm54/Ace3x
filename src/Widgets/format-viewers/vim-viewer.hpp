/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_

#include <QHBoxLayout>
#include <QListWidget>
#include <QWidget>

#include "format-readers/vif-mesh.hpp"
#include "ui_VIMViewer.h"
#include "widgets/format-viewers/viewer.hpp"

class VIMViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit VIMViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    Ui::VIMViewer ui_;
    VifMesh vim_;
    const TreeEntry *item_;

private slots:
    void sub0Changed();
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_
