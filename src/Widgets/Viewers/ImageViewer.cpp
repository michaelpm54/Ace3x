/* SPDX-License-Identifier: GPLv3-or-later */

#include "ImageViewer.hpp"

#include <QDebug>
#include <QImage>
#include <QKeyEvent>

#include "FrameDecoder.hpp"
#include "PegEntry.hpp"

ImageViewer::ImageViewer(QWidget *parent)
        : QWidget(parent)
{
	ui.setupUi(this);

	setWindowTitle("Ace3x - Image Viewer");

	connect(ui.prev, &QPushButton::pressed, this, [this]() { prevFrame(); });
	connect(ui.next, &QPushButton::pressed, this, [this]() { nextFrame(); });
}

void ImageViewer::activate(const Entry *item)
{
	if (!item)
		return;

	show();

        if (item->getExtension() == "peg") {
                mPeg        = static_cast<const PegEntry *>(item);
                mFrameIndex = 0;
        } else if (item->getParent() && (item->getParent()->getExtension() == "peg")) {
                mPeg        = static_cast<const PegEntry *>(item->getParent());
                mFrameIndex = item->getIndex();
	}

        if (!mPeg || (mFrameIndex >= mPeg->count()))
		throw std::runtime_error("Failed to activate ImageViewer for item");

        ui.pegLineEdit->setText(mPeg->getFilename());
        ui.frameLineEdit->setText(mPeg->getChild(mFrameIndex)->getFilename());
        ui.numFrames->setText(QString::number(mPeg->count()));
	ui.frameIndex->setText(QString::number(mFrameIndex + 1));

        update();
}

void ImageViewer::keyPressEvent(QKeyEvent *event)
{
        QWidget::keyPressEvent(event);
	switch (event->type()) {
		case QKeyEvent::KeyPress:
			switch (event->key()) {
				case Qt::Key::Key_N:
					nextFrame();
					break;
				case Qt::Key::Key_B:
					prevFrame();
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

void ImageViewer::nextFrame()
{
        mFrameIndex = (mFrameIndex + 1) % mPeg->count();
	ui.frameIndex->setText(QString::number(mFrameIndex + 1));
	update();
}

void ImageViewer::prevFrame()
{
        mFrameIndex = ((mFrameIndex - 1) + mPeg->count()) % mPeg->count();
        ui.frameIndex->setText(QString::number(mFrameIndex + 1));
	update();
}

void ImageViewer::update()
{
        const auto frame = mPeg->getFrame(mFrameIndex);
        ui.frameLineEdit->setText(frame.filename);

        std::vector<std::uint32_t> pixels(frame.width * frame.height);
        decodeFrame(
                reinterpret_cast<std::uint32_t *>(pixels.data()),
                mPeg->getChild(mFrameIndex)->getData(),
                frame.width, frame.height, frame.format);

        auto image = QImage(reinterpret_cast<const std::uint8_t *>(pixels.data()), frame.width, frame.height, QImage::Format_ARGB32);

        ui.label->setPixmap(QPixmap::fromImage(image));
}

bool ImageViewer::shouldBeEnabled(const Entry *item) const
{
        if (item->getExtension() == "peg")
                return item->count() != 0;

        return true;
}
