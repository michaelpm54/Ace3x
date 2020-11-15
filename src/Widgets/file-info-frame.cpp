/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/file-info-frame.hpp"

#include <spdlog/spdlog.h>

#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>

#include "vfs/vfs-entry.hpp"
#include "widgets/format-viewers/viewer.hpp"

FileInfoFrame::FileInfoFrame(QWidget *parent)
    : QFrame(parent)
    , filename_(new QLineEdit())
    , size_(new QLineEdit())
    , type_(new QLineEdit())
    , save_(new QPushButton("Save"))
    , view_(new QPushButton("View"))
{
    view_->hide();

    auto layout = new QFormLayout();
    setLayout(layout);

    filename_->setReadOnly(true);
    size_->setReadOnly(true);
    type_->setReadOnly(true);
    layout->addRow("Filename", filename_);
    layout->addRow("Size", size_);
    layout->addRow("Type", type_);
    layout->addWidget(save_);
    layout->addWidget(view_);

    save_->hide();

    connect(save_, &QPushButton::released, this, &FileInfoFrame::saveButtonClicked);
    connect(view_, &QPushButton::released, this, &FileInfoFrame::viewButtonClicked);
}

void FileInfoFrame::setItem(VfsEntry *item)
{
    save_->show();

    item_ = item;

    auto ext = QString::fromStdString(item->extension);

    filename_->setText(QString::fromStdString(item->name));
    filename_->setToolTip(QString::fromStdString(item->relative_path));
    size_->setText(QLocale::system().formattedDataSize(item->size, 2, nullptr));
    type_->setText(ext);

    if (viewers_.count(ext)) {
        if (viewers_[ext]->shouldBeEnabled(item)) {
            view_->show();
        }
        else {
            view_->hide();
        }
    }
    else {
        view_->hide();
    }
}

void FileInfoFrame::addViewer(QString ext, Viewer *viewer)
{
    viewers_[ext] = viewer;
}

void FileInfoFrame::saveButtonClicked()
{
    const auto filename = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + QString::fromStdString(item_->name));

    if (filename.isEmpty()) {
        return;
    }

    QFile file(filename);

    if (false == file.open(QIODevice::WriteOnly)) {
        spdlog::error("Failed to open file '{}' for writing: {}", filename.toStdString(), file.errorString().toStdString());
    }
    else if (-1 == file.write(reinterpret_cast<const char *>(item_->data), item_->size)) {
        spdlog::error("Failed to write data for '{}': ", filename.toStdString(), file.errorString().toStdString());
    }
    else {
        spdlog::info("Wrote file '{}'", filename.toStdString());
    }
}

void FileInfoFrame::viewButtonClicked()
{
    try {
        emit viewClicked(item_);
        viewers_[QString::fromStdString(item_->extension)]->activate(item_);
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(e.what()), QMessageBox::Ok);
    }
}

void FileInfoFrame::clear()
{
    filename_->clear();
    size_->clear();
    type_->clear();
    save_->hide();
    view_->hide();
}
