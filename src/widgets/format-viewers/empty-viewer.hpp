/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_EMPTY_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_EMPTY_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "widgets/format-viewers/viewer.hpp"

class Ui_EmptyViewer;

class EmptyViewer : public Viewer {
    Q_OBJECT
public:
    EmptyViewer(QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

private:
    std::unique_ptr<Ui_EmptyViewer> ui_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWER_EMPTY_VIEWER_HPP_
