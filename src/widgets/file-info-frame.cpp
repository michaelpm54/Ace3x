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

#include "ui_file-info-frame.h"
#include "vfs/vfs-entry.hpp"
#include "widgets/format-viewers/viewer.hpp"

FileInfoFrame::FileInfoFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui_FileInfoFrame())
{
    ui->setupUi(this);

    connect(ui->save_btn, &QPushButton::released, this, &FileInfoFrame::save_btn_clicked);
    connect(ui->view_btn, &QPushButton::released, this, &FileInfoFrame::view_btn_clicked);
}

void FileInfoFrame::set_item(VfsEntry *item)
{
    item_ = item;

    auto ext = QString::fromStdString(item->extension);

    ui->archive_name->setText(QString::fromStdString(item->root->name));
    ui->filename->setText(QString::fromStdString(item->name));
    ui->filename->setToolTip(QString::fromStdString(item->relative_path));
    ui->filesize->setText(QLocale::system().formattedDataSize(item->size, 2, nullptr));
    ui->extension->setText(ext);

    ui->view_btn->setEnabled(false);
    ui->save_btn->setEnabled(true);
}

void FileInfoFrame::enable_view()
{
    ui->view_btn->setEnabled(true);
}

void FileInfoFrame::save_btn_clicked()
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

void FileInfoFrame::view_btn_clicked()
{
    emit view_clicked(item_);
}

void FileInfoFrame::clear()
{
    ui->archive_name->clear();
    ui->filename->clear();
    ui->filesize->clear();
    ui->extension->clear();
    ui->view_btn->setEnabled(false);
    ui->save_btn->setEnabled(false);
}
