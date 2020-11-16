/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "widgets/format-viewers/viewer.hpp"

namespace Ui {
class Vf2Viewer;
}

class Vfs;

class Vf2Viewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit Vf2Viewer(Vfs *vfs, QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

signals:
    void request_load(const QString &path);

private:
    std::unique_ptr<Ui::Vf2Viewer> ui_;
    Vfs *vfs_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_
