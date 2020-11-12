/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_

class TreeEntry;

class Viewer {
public:
    virtual ~Viewer() = default;
    virtual void activate(const TreeEntry *item) = 0;
    virtual bool shouldBeEnabled(const TreeEntry *item) const = 0;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_VIEWER_HPP_
