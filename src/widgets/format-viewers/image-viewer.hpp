/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "widgets/format-viewers/viewer.hpp"

namespace Ui {
class ImageViewer;
}

namespace ace3x::peg {
struct Image;
}

class ImageViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateImage();

private slots:
    void nextFrame();
    void prevFrame();
    void saveFrame();

private:
    std::unique_ptr<Ui::ImageViewer> ui_;
    const VfsEntry *peg_ {nullptr};
    std::uint16_t current_frame_index_ {0};
    QString current_frame_name_;
    std::vector<ace3x::peg::Image> images_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_IMAGE_VIEWER_HPP_
