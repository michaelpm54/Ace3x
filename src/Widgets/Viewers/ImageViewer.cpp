/* SPDX-License-Identifier: GPLv3-or-later */

#include "ImageViewer.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QKeyEvent>

#include "tree-entry/peg-entry.hpp"

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

void ImageViewer::activate(const TreeEntry *item)
{
    if (!item) {
        return;
    }

    show();

    if (item->getExtension() == ".peg") {
        mPeg = static_cast<const PegEntry *>(item);
        mFrameIndex = 0;
    }
    else if (item->getParent() && (item->getParent()->getExtension() == ".peg")) {
        mPeg = static_cast<const PegEntry *>(item->getParent());
        mFrameIndex = item->getIndex();
    }

    if (!mPeg) {
        throw std::runtime_error("PEG is null");
    }

    if (mFrameIndex >= mPeg->get_num_children()) {
        throw std::runtime_error("PEG doesn't have frame for index: " + std::to_string(mFrameIndex));
    }

    ui.pegLineEdit->setText(QString::fromStdString(mPeg->getFilename()));
    ui.frameLineEdit->setText(QString::fromStdString(mPeg->getChild(mFrameIndex)->getFilename()));
    ui.numFrames->setText(QString::number(mPeg->get_num_children()));
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
    mFrameIndex = (mFrameIndex + 1) % mPeg->get_num_children();
    ui.frameIndex->setText(QString::number(mFrameIndex + 1));
    update();
}

void ImageViewer::prevFrame()
{
    mFrameIndex = ((mFrameIndex - 1) + mPeg->get_num_children()) % mPeg->get_num_children();
    ui.frameIndex->setText(QString::number(mFrameIndex + 1));
    update();
}

void ImageViewer::update()
{
    const auto peg_image = mPeg->getImage(mFrameIndex);
    const auto qt_image = QImage(reinterpret_cast<const std::uint8_t *>(peg_image.pixels.data()), peg_image.width, peg_image.height, QImage::Format_ARGB32);

    m_frameName = QString::fromStdString(peg_image.filename);

    ui.frameLineEdit->setText(QString::fromStdString(peg_image.filename));
    ui.label->setPixmap(QPixmap::fromImage(qt_image));
}

bool ImageViewer::shouldBeEnabled(const TreeEntry *item) const
{
    if (item->getExtension() == ".peg") {
        return (item->get_num_children() != 0);
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
