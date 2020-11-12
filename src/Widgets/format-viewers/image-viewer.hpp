/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_

#include <QWidget>

#include "ui_ImageViewer.h"
#include "widgets/format-viewers/viewer.hpp"

class TreeEntry;
class PegEntry;

class ImageViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void update();

private slots:
    void nextFrame();
    void prevFrame();
    void saveFrame();

private:
    Ui::ImageViewer ui;
    std::uint16_t mFrameIndex {0};
    const PegEntry *mPeg {nullptr};
    QString m_frameName;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_
