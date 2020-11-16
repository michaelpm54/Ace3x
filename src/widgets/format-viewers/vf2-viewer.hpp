/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "widgets/format-viewers/viewer.hpp"

class QGraphicsScene;

namespace Ui {
class Vf2Viewer;
}

class Vfs;

class Vf2Viewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    Vf2Viewer(Vfs *vfs, QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

signals:
    void request_load(const QString &path);

private slots:
    void update_font_scene();

private:
    std::unique_ptr<Ui::Vf2Viewer> ui_;
    Vfs *vfs_;
    QGraphicsScene *font_scene_;

    struct FontChar {
        int min_width;
        int max_width;
        std::uint32_t pixel_offset;
        std::uint32_t kerning_back_pixels_width;
    };

    int font_height_;
    int first_ascii_;

    std::vector<QRect> char_rects_;

    std::unique_ptr<QPixmap> font_pixmap_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VF2_VIEWER_HPP_
