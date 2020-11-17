/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_

struct VfsEntry;

class Viewer : public QWidget {
public:
    using QWidget::QWidget;
    virtual ~Viewer() = default;
    virtual void activate(const VfsEntry *item) = 0;
    virtual bool shouldBeEnabled(const VfsEntry *item) const = 0;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
