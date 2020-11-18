/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_

#include <QWidget>
#include <string>

struct VfsEntry;

class Viewer : public QWidget {
    Q_OBJECT

public:
    Viewer(QWidget *parent = nullptr);
    virtual ~Viewer() = default;
    virtual void activate(const VfsEntry *item) = 0;
    virtual bool shouldBeEnabled(const VfsEntry *item) const = 0;

signals:
    void referenced_file(const std::string &filename);
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
