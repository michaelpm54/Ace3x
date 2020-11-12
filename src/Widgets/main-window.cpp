/* SPDX-License-Identifier: GPLv3-or-later */

#include "Widgets/main-window.hpp"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeView>
#include <filesystem>
#include <regex>

#include "fs.hpp"
#include "tree-entries/tree-entry-sort-proxy.hpp"
#include "tree-entries/vpp-entry.hpp"
#include "tree-model.hpp"
#include "widgets/file-info-frame.hpp"
#include "widgets/format-viewers/image-viewer.hpp"
#include "widgets/format-viewers/p3d-viewer.hpp"
#include "widgets/format-viewers/plaintext-viewer.hpp"
#include "widgets/format-viewers/vim-viewer.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mFileView(new QTreeView())
    , mFileViewModel(new TreeModel())
    , mTreeEntrySortProxy(new TreeEntrySortProxy())
    , mFileInspector(new FileInfoFrame())
    , mLog(new QTextEdit())
    , mImageViewer(new ImageViewer())
    , mPlaintextViewer(new PlaintextViewer())
    , mP3DViewer(new P3DViewer())
    , mVIMViewer(new VIMViewer())
    , mLastPath(QDir::currentPath())
{
    setupActions();

    resize(1280, 720);

    auto centralWidget = new QWidget();
    auto layout = new QGridLayout();
    auto splitter0 = new QSplitter(Qt::Orientation::Vertical);
    auto splitter1 = new QSplitter(Qt::Orientation::Horizontal);

    mLog->setReadOnly(true);

    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);

    layout->addWidget(splitter0);

    splitter0->addWidget(splitter1);
    splitter0->addWidget(mLog);
    splitter0->setChildrenCollapsible(false);

    splitter1->addWidget(mFileView);
    splitter1->addWidget(mFileInspector);
    splitter1->setChildrenCollapsible(false);

    splitter0->setStretchFactor(0, 3);
    splitter0->setStretchFactor(1, 1);

    mImageViewer->hide();

    mTreeEntrySortProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    mTreeEntrySortProxy->setSourceModel(mFileViewModel);

    QSizePolicy leftPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    leftPolicy.setHorizontalStretch(2);
    mFileView->setSizePolicy(leftPolicy);
    mFileView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    mFileView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    mFileView->setAlternatingRowColors(true);
    mFileView->setModel(mTreeEntrySortProxy);

    connect(mFileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateSelection);
    connect(mFileView, &QTreeView::expanded, this, [this]() {
        mFileView->resizeColumnToContents(0);
    });

    QSizePolicy rightPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    rightPolicy.setHorizontalStretch(1);
    mFileInspector->setSizePolicy(rightPolicy);

    mFileInspector->addViewer(".peg", mImageViewer);
    mFileInspector->addViewer(".tga", mImageViewer);
    mFileInspector->addViewer(".vbm", mImageViewer);
    mFileInspector->addViewer(".tbl", mPlaintextViewer);
    mFileInspector->addViewer(".arr", mPlaintextViewer);
    mFileInspector->addViewer(".p3d", mP3DViewer);
    mFileInspector->addViewer(".vim", mVIMViewer);
}

MainWindow::~MainWindow()
{
    delete mFileViewModel;
    delete mTreeEntrySortProxy;
    delete mImageViewer;
}

void MainWindow::setupActions()
{
    auto mb = new QMenuBar();

    auto file = new QMenu("File");
    file->addAction("Open", this, &MainWindow::actionOpen, QKeySequence("Ctrl+O"));
    file->addAction("Close", this, &MainWindow::actionClose, QKeySequence("Ctrl+W"));
    file->addSeparator();
    file->addAction("Quit", this, &MainWindow::actionQuit, QKeySequence("Ctrl+Q"));

    mb->addMenu(file);

    setMenuBar(mb);
}

void MainWindow::updateSelection(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.isEmpty()) {
        return;
    }

    qDebug() << selected.indexes().first();

    TreeEntry *entry = mFileViewModel->itemFromIndex(mTreeEntrySortProxy->mapToSource(selected.indexes().first()));

    mFileInspector->setItem(entry);
}

void MainWindow::actionOpen()
{
    actionClose();

    load(QFileDialog::getOpenFileName(this, "Open VPP", mLastPath, "Archive File (*.VPP)"));
}

void MainWindow::actionClose()
{
    mLog->clear();
    mFileViewModel->clear();
    mFileInspector->clear();
}

void MainWindow::actionQuit()
{
    QApplication::quit();
}

void MainWindow::load(const QString &path)
{
    if (path.isEmpty())
        return;

    mLastPath = QFileInfo(path).dir().absolutePath();

    int num_loaded = mFileViewModel->load(path);

    // If there is only one top-level archive, expand it.
    // Don't do this for multiple top-level archives because it's messy.
    // Let the user expand them on their own.
    if (num_loaded == 1) {
        mFileView->expandToDepth(0);
    }

    mFileView->resizeColumnToContents(0);
    mFileView->setSortingEnabled(true);
}
