/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_

#include <QHBoxLayout>
#include <QListWidget>
#include <QWidget>
#include <memory>

#include "format-readers/vif-mesh.hpp"
#include "widgets/format-viewers/viewer.hpp"

namespace Ui {
class VIMViewer;
}

class VIMViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit VIMViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    std::unique_ptr<Ui::VIMViewer> ui_;
    VifMesh vim_;
    const TreeEntry *item_;

private slots:
    void sub0Changed();
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VIM_VIEWER_HPP_
