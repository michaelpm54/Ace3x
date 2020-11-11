/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QWidget>

#include "Viewer.hpp"

#include "ui_ImageViewer.h"

class Entry;
class PegEntry;

class ImageViewer : public QWidget, public Viewer {
	Q_OBJECT
public:
	explicit ImageViewer(QWidget *parent = nullptr);

	void activate(const Entry *item) override;
        bool shouldBeEnabled(const Entry *item) const override;

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
	std::uint16_t mFrameIndex { 0 };
        const PegEntry *mPeg{ nullptr };
        QString m_frameName;
};

#endif // IMAGEVIEWER_HPP
