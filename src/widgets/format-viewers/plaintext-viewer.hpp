/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "ui_plaintext-viewer.h"
#include "widgets/format-viewers/viewer.hpp"

class TreeEntry;

class PlaintextViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit PlaintextViewer(QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

private:
    std::unique_ptr<Ui::PlaintextViewer> ui_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_
