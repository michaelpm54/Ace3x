/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/image-viewer.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QKeyEvent>

#include "tree-entries/peg-entry.hpp"


ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
{
    ui_.setupUi(this);

    setWindowTitle("Ace3x - Image Viewer");

    connect(ui_.prev, &QPushButton::pressed, this, [this]() {
        prevFrame();
    });
    connect(ui_.next, &QPushButton::pressed, this, [this]() {
        nextFrame();
    });
    connect(ui_.save, &QPushButton::pressed, this, [this]() {
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
        peg_ = static_cast<const PegEntry *>(item);
        current_frame_index_ = 0;
    }
    else if (item->getParent() && (item->getParent()->getExtension() == ".peg")) {
        peg_ = static_cast<const PegEntry *>(item->getParent());
        current_frame_index_ = item->getIndex();
    }

    if (!peg_) {
        throw std::runtime_error("PEG is null");
    }

    if (current_frame_index_ >= peg_->get_num_children()) {
        throw std::runtime_error("PEG doesn't have frame for index: " + std::to_string(current_frame_index_));
    }

    ui_.pegLineEdit->setText(QString::fromStdString(peg_->getFilename()));
    ui_.frameLineEdit->setText(QString::fromStdString(peg_->getChild(current_frame_index_)->getFilename()));
    ui_.numFrames->setText(QString::number(peg_->get_num_children()));
    ui_.frameIndex->setText(QString::number(current_frame_index_ + 1));

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
    current_frame_index_ = (current_frame_index_ + 1) % peg_->get_num_children();
    ui_.frameIndex->setText(QString::number(current_frame_index_ + 1));
    update();
}

void ImageViewer::prevFrame()
{
    current_frame_index_ = ((current_frame_index_ - 1) + peg_->get_num_children()) % peg_->get_num_children();
    ui_.frameIndex->setText(QString::number(current_frame_index_ + 1));
    update();
}

void ImageViewer::update()
{
    const auto peg_image = peg_->getImage(current_frame_index_);
    const auto qt_image = QImage(reinterpret_cast<const std::uint8_t *>(peg_image.pixels.data()), peg_image.width, peg_image.height, QImage::Format_ARGB32);

    current_frame_name_ = QString::fromStdString(peg_image.filename);

    ui_.frameLineEdit->setText(QString::fromStdString(peg_image.filename));
    ui_.label->setPixmap(QPixmap::fromImage(qt_image));
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
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + current_frame_name_ + ".png");
    if (fileName.isEmpty()) {
        std::clog << "[Info] Cancelled save" << std::endl;
        return;
    }

    QFile file(fileName);

    if (false == file.open(QIODevice::WriteOnly)) {
        std::clog << "[Error] Failed to open file for writing: " << fileName.toStdString() << std::endl;
    }
    else if (!ui_.label->pixmap()->save(fileName, "PNG")) {
        std::clog << "[Error] Failed to write data for " << fileName.toStdString() << std::endl;
    }
    else {
        std::clog << "[Info] Wrote file " << fileName.toStdString() << std::endl;
    }
}
