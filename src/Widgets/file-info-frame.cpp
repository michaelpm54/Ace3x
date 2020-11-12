/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/file-info-frame.hpp"

#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>

#include "tree-entries/tree-entry.hpp"
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

void FileInfoFrame::setItem(const TreeEntry *const item)
{
    save_->show();

    item_ = item;

    auto ext = QString::fromStdString(item->getExtension());

    filename_->setText(QString::fromStdString(item->getFilename()));
    filename_->setToolTip(QString::fromStdString(item->getPath()));
    size_->setText(QLocale::system().formattedDataSize(item->getSize(), 2, nullptr));
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
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + QString::fromStdString(item_->getFilename()));
    if (fileName.isEmpty()) {
        std::clog << "[Info] Cancelled save" << std::endl;
        return;
    }

    QFile file(fileName);

    if (false == file.open(QIODevice::WriteOnly)) {
        std::clog << "[Error] Failed to open file for writing: " << fileName.toStdString() << std::endl;
    }
    else if (-1 == file.write(reinterpret_cast<const char *>(item_->getData()), item_->getSize())) {
        std::clog << "[Error] Failed to write data for " << fileName.toStdString() << std::endl;
    }
    else {
        std::clog << "[Info] Wrote file " << fileName.toStdString() << std::endl;
    }
}

void FileInfoFrame::viewButtonClicked()
{
    try {
        viewers_[QString::fromStdString(item_->getExtension())]->activate(item_);
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
