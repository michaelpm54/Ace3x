/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef P3DVIEWER_HPP
#define P3DVIEWER_HPP

#include <QWidget>

#include "Viewer.hpp"

#include "ui_P3DViewer.h"

class Entry;

class P3DViewer : public QWidget, public Viewer {
        Q_OBJECT
public:
        explicit P3DViewer(QWidget *parent = nullptr);

        void activate(const Entry *item) override;
        bool shouldBeEnabled(const Entry *item) const override;

private:
        Ui::P3DViewer ui;
};

#endif // P3DVIEWER_HPP
