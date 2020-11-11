/* SPDX-License-Identifier: GPLv3-or-later */

#include "FileInspector.hpp"

#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>

#include "Widgets/Viewers/Viewer.hpp"
#include "tree-entry/tree-entry.hpp"

FileInspector::FileInspector(QWidget *parent)
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

    connect(mSave, &QPushButton::released, this, &FileInspector::saveButtonClicked);
    connect(mView, &QPushButton::released, this, &FileInspector::viewButtonClicked);
}

void FileInspector::setItem(const TreeEntry *const item)
{
    mSave->show();

    mItem = item;

    auto ext = item->getExtension();

    mFilename->setText(item->getFilename());
    mFilename->setToolTip(item->getPath());
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

void FileInspector::addViewer(QString ext, Viewer *viewer)
{
    mViewers[ext] = viewer;
}

void FileInspector::saveButtonClicked()
{
    const auto fileName = QFileDialog::getSaveFileName(this, "Save File", QDir::currentPath() + '/' + mItem->getFilename());
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

void FileInspector::viewButtonClicked()
{
    try {
        mViewers[mItem->getExtension()]->activate(mItem);
    }
    catch (const std::runtime_error &e) {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(e.what()), QMessageBox::Ok);
    }
}

void FileInspector::clear()
{
    mFilename->clear();
    mSize->clear();
    mType->clear();
    mSave->hide();
    mView->hide();
}
