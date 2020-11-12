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
    , mFilename(new QLineEdit())
    , mSize(new QLineEdit())
    , mType(new QLineEdit())
    , mSave(new QPushButton("Save"))
    , mView(new QPushButton("View"))
{
    mView->hide();

    auto layout = new QFormLayout();
    setLayout(layout);

    mFilename->setReadOnly(true);
    mSize->setReadOnly(true);
    mType->setReadOnly(true);
    layout->addRow("Filename", mFilename);
    layout->addRow("Size", mSize);
    layout->addRow("Type", mType);
    layout->addWidget(mSave);
    layout->addWidget(mView);

    mSave->hide();

    connect(mSave, &QPushButton::released, this, &FileInfoFrame::saveButtonClicked);
    connect(mView, &QPushButton::released, this, &FileInfoFrame::viewButtonClicked);
}

void FileInfoFrame::setItem(const TreeEntry *const item)
{
    mSave->show();

    mItem = item;

    auto ext = QString::fromStdString(item->getExtension());

    mFilename->setText(QString::fromStdString(item->getFilename()));
    mFilename->setToolTip(QString::fromStdString(item->getPath()));
    mSize->setText(QLocale::system().formattedDataSize(item->getSize(), 2, nullptr));
    mType->setText(ext);

    if (mViewers.count(ext)) {
        if (mViewers[ext]->shouldBeEnabled(item)) {
            mView->show();
        }
        else {
            mView->hide();
        }
    }
    else {
        mView->hide();
    }
}

void FileInfoFrame::addViewer(QString ext, Viewer *viewer)
{
    mViewers[ext] = viewer;
}

void FileInfoFrame::saveButtonClicked()
{
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + QString::fromStdString(mItem->getFilename()));
    if (fileName.isEmpty()) {
        std::clog << "[Info] Cancelled save" << std::endl;
        return;
    }

    QFile file(fileName);

    if (false == file.open(QIODevice::WriteOnly)) {
        std::clog << "[Error] Failed to open file for writing: " << fileName.toStdString() << std::endl;
    }
    else if (-1 == file.write(reinterpret_cast<const char *>(mItem->getData()), mItem->getSize())) {
        std::clog << "[Error] Failed to write data for " << fileName.toStdString() << std::endl;
    }
    else {
        std::clog << "[Info] Wrote file " << fileName.toStdString() << std::endl;
    }
}

void FileInfoFrame::viewButtonClicked()
{
    try {
        mViewers[QString::fromStdString(mItem->getExtension())]->activate(mItem);
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(e.what()), QMessageBox::Ok);
    }
}

void FileInfoFrame::clear()
{
    mFilename->clear();
    mSize->clear();
    mType->clear();
    mSave->hide();
    mView->hide();
}
