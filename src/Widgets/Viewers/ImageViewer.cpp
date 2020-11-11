/* SPDX-License-Identifier: GPLv3-or-later */

#include "ImageViewer.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QKeyEvent>

#include "FrameDecoder.hpp"
#include "PegEntry.hpp"

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    setWindowTitle("Ace3x - Image Viewer");

    connect(ui.prev, &QPushButton::pressed, this, [this]() {
        prevFrame();
    });
    connect(ui.next, &QPushButton::pressed, this, [this]() {
        nextFrame();
    });
    connect(ui.save, &QPushButton::pressed, this, [this]() {
        saveFrame();
    });
}

void ImageViewer::activate(const Entry *item)
{
    if (!item) {
        return;
    }

    show();

    if (item->getExtension() == "peg") {
        mPeg = static_cast<const PegEntry *>(item);
        mFrameIndex = 0;
    }
    else if (item->getParent() && (item->getParent()->getExtension() == "peg")) {
        mPeg = static_cast<const PegEntry *>(item->getParent());
        mFrameIndex = item->getIndex();
    }

    if (!mPeg || (mFrameIndex >= mPeg->count())) {
        throw std::runtime_error("Failed to activate ImageViewer for item");
    }

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
        case QKeyEvent::KeyPress: {
            switch (event->key()) {
                case Qt::Key::Key_N: {
                    nextFrame();
                    break;
                }
                case Qt::Key::Key_B: {
                    prevFrame();
                    break;
                }
                default: {
                    break;
                }
            }
            break;
        }
        default: {
            break;
        }
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
    m_frameName = frame.filename;
    ui.frameLineEdit->setText(frame.filename);

    std::vector<std::uint32_t> pixels(frame.width * frame.height);
    decodeFrame(
        reinterpret_cast<std::uint32_t *>(pixels.data()),
        mPeg->getChild(mFrameIndex)->getData(),
        frame.width,
        frame.height,
        frame.format);

    auto image = QImage(reinterpret_cast<const std::uint8_t *>(pixels.data()), frame.width, frame.height, QImage::Format_ARGB32);

    ui.label->setPixmap(QPixmap::fromImage(image));
}

bool ImageViewer::shouldBeEnabled(const Entry *item) const
{
    if (item->getExtension() == "peg") {
        return (item->count() != 0);
    }

    return (true);
}

void ImageViewer::saveFrame()
{
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + m_frameName + ".png");
    if (fileName.isEmpty()) {
        std::clog << "[Info] Cancelled save" << std::endl;
        return;
    }

    QFile file(fileName);

    if (false == file.open(QIODevice::WriteOnly)) {
        std::clog << "[Error] Failed to open file for writing: " << fileName.toStdString() << std::endl;
    }
    else if (!ui.label->pixmap()->save(fileName, "PNG")) {
        std::clog << "[Error] Failed to write data for " << fileName.toStdString() << std::endl;
    }
    else {
        std::clog << "[Info] Wrote file " << fileName.toStdString() << std::endl;
    }
}
