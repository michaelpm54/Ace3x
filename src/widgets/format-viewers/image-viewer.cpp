/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/image-viewer.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QKeyEvent>

#include "format-readers/peg.hpp"
#include "ui_image-viewer.h"
#include "vfs/vfs-entry.hpp"

ImageViewer::ImageViewer(QWidget *parent)
    : Viewer(parent)
    , ui_(new Ui::ImageViewer())
{
    ui_->setupUi(this);

    setWindowTitle("Ace3x - Image Viewer");

    connect(ui_->prev, &QPushButton::pressed, this, [this]() {
        prevFrame();
    });
    connect(ui_->next, &QPushButton::pressed, this, [this]() {
        nextFrame();
    });
    connect(ui_->save, &QPushButton::pressed, this, [this]() {
        saveFrame();
    });
}

void ImageViewer::activate(const VfsEntry *item)
{
    assert(item);

    show();

    if (item->extension == ".peg") {
        current_frame_index_ = 0;
        peg_ = item;
    }
    else if (item->parent && (item->parent->extension == ".peg")) {
        current_frame_index_ = item->index;
        peg_ = item->parent;
    }

    assert(peg_);

    assert(current_frame_index_ < peg_->entries.size());

    ui_->peg_name->setText(QString::fromStdString(peg_->name));
    ui_->image_name->setText(QString::fromStdString(peg_->entries[current_frame_index_]->name));
    ui_->image_index->setText(QString("%1/%2").arg(current_frame_index_ + 1).arg(peg_->entries.size()));

    images_ = ace3x::peg::get_images(peg_->data);

    updateImage();
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
    current_frame_index_ = (current_frame_index_ + 1) % peg_->entries.size();
    updateImage();
}

void ImageViewer::prevFrame()
{
    current_frame_index_ = ((current_frame_index_ - 1) + peg_->entries.size()) % peg_->entries.size();
    updateImage();
}

void ImageViewer::updateImage()
{
    const auto &peg_image = images_[current_frame_index_];
    const auto qt_image = QImage(reinterpret_cast<const unsigned char *>(peg_image.pixels.data()), peg_image.width, peg_image.height, QImage::Format_ARGB32);

    current_frame_name_ = QString::fromStdString(peg_image.filename);

    ui_->image_name->setText(QString::fromStdString(peg_image.filename));
    ui_->image_size->setText(QLocale::system().formattedDataSize(peg_->entries[current_frame_index_]->size, 2, nullptr));
    ui_->image_index->setText(QString("%1/%2").arg(current_frame_index_ + 1).arg(peg_->entries.size()));
    ui_->image_label->setPixmap(QPixmap::fromImage(qt_image));
}

bool ImageViewer::shouldBeEnabled(const VfsEntry *item) const
{
    return item->extension == ".peg" || (item->parent && item->parent->extension == ".peg");
}

void ImageViewer::saveFrame()
{
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + current_frame_name_ + ".png");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (false == file.open(QIODevice::WriteOnly)) {
        spdlog::error("Image viewer: Failed to open file for writing: '{}'", fileName.toStdString());
    }
    else if (!ui_->image_label->pixmap()->save(fileName, "PNG")) {
        spdlog::error("Image viewer: Failed to write data for '{}'", fileName.toStdString());
    }
    else {
        spdlog::info("Wrote file '{}'", fileName.toStdString());
    }
}
