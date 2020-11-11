/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VIEWER_HPP
#define VIEWER_HPP

class TreeEntry;

class Viewer {
public:
    virtual ~Viewer();
    virtual void activate(const TreeEntry *item) = 0;
    virtual bool shouldBeEnabled(const TreeEntry *item) const = 0;
};

#endif    // VIEWER_HPP
