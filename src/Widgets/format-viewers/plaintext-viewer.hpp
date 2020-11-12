/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_

#include <QWidget>

#include "ui_PlaintextViewer.h"
#include "widgets/format-viewers/viewer.hpp"

class TreeEntry;

class PlaintextViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit PlaintextViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    Ui::PlaintextViewer ui_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_PLAINTEXT_VIEWER_HPP_
