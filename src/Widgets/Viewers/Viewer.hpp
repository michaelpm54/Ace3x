/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VIEWER_HPP
#define VIEWER_HPP

class Entry;

class Viewer {
public:
    virtual ~Viewer();
    virtual void activate(const Entry *item) = 0;
    virtual bool shouldBeEnabled(const Entry *item) const = 0;
};

#endif    // VIEWER_HPP
