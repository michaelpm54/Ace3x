/* SPDX-License-Identifier: GPLv3-or-later */

#include "Widgets/main-window.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextEdit>
#include <QTextStream>
#include <QTreeView>

#include "tree-model/sort-proxy.hpp"
#include "tree-model/tree-model.hpp"
#include "vfs/mmap-vfs.hpp"
#include "widgets/file-info-frame.hpp"
#include "widgets/format-viewers/image-viewer.hpp"
#include "widgets/format-viewers/p3d-viewer.hpp"
#include "widgets/format-viewers/plaintext-viewer.hpp"
#include "widgets/format-viewers/vim-viewer.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , vfs_(std::make_unique<MmapVfs>())
    , log_(new QPlainTextEdit())
    , tree_view_(new QTreeView())
    , tree_model_(new TreeModel(vfs_.get()))
    , tree_sort_proxy_(new TreeEntrySortProxy())
    , file_info_view_(new FileInfoFrame())
    , image_viewer_(new ImageViewer())
    , plaintext_viewer_(new PlaintextViewer())
    , p3d_viewer_(new P3DViewer())
    , vim_viewer_(new VIMViewer())
    , last_open_path_(QDir::currentPath())
{
    setupActions();

    resize(1280, 720);

    auto centralWidget = new QWidget();
    auto layout = new QGridLayout();
    auto splitter0 = new QSplitter(Qt::Orientation::Vertical);
    auto splitter1 = new QSplitter(Qt::Orientation::Horizontal);

    log_->setReadOnly(true);

    QFont monospace_font;
    monospace_font.setPointSize(10);
    monospace_font.setBold(false);
    monospace_font.setStyleHint(QFont::Monospace);
    monospace_font.setFamily("System,Fixedsys Standard,Consolas");
    log_->document()->setDefaultFont(monospace_font);

    setCentralWidget(centralWidget);
    centralWidget->setLayout(layout);

    layout->addWidget(splitter0);

    splitter0->addWidget(splitter1);
    splitter0->addWidget(log_);
    splitter0->setChildrenCollapsible(false);

    splitter1->addWidget(tree_view_);
    splitter1->addWidget(file_info_view_);
    splitter1->setChildrenCollapsible(false);

    splitter0->setStretchFactor(0, 3);
    splitter0->setStretchFactor(1, 1);

    image_viewer_->hide();

    tree_sort_proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    tree_sort_proxy_->setSourceModel(tree_model_);

    QSizePolicy leftPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    leftPolicy.setHorizontalStretch(2);
    tree_view_->setSizePolicy(leftPolicy);
    tree_view_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    tree_view_->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    tree_view_->setAlternatingRowColors(true);
    tree_view_->setModel(tree_sort_proxy_);

    connect(tree_view_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateSelection);
    connect(tree_view_, &QTreeView::expanded, this, [this]() {
        tree_view_->resizeColumnToContents(0);
    });

    tree_view_->setUniformRowHeights(true);

    QSizePolicy rightPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    rightPolicy.setHorizontalStretch(1);
    file_info_view_->setSizePolicy(rightPolicy);

    file_info_view_->addViewer(".peg", image_viewer_);
    file_info_view_->addViewer(".tga", image_viewer_);
    file_info_view_->addViewer(".vbm", image_viewer_);
    file_info_view_->addViewer(".tbl", plaintext_viewer_);
    file_info_view_->addViewer(".arr", plaintext_viewer_);
    file_info_view_->addViewer(".p3d", p3d_viewer_);
    file_info_view_->addViewer(".vim", vim_viewer_);

    if (std::filesystem::exists("settings.txt")) {
        QFile file("settings.txt");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            stream >> last_open_path_;
        }
        file.close();
    }
}

MainWindow::~MainWindow()
{
    delete tree_model_;
    delete tree_sort_proxy_;
    delete image_viewer_;
    delete p3d_viewer_;
    delete vim_viewer_;
    delete plaintext_viewer_;
}

QPlainTextEdit *MainWindow::getLog()
{
    return log_;
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

    auto *entry = tree_model_->itemFromIndex(tree_sort_proxy_->mapToSource(selected.indexes().first()));

    file_info_view_->setItem(entry);
}

void MainWindow::actionOpen()
{
    load(QFileDialog::getOpenFileName(this, "Open VPP", last_open_path_, "Archive File (*.VPP)"));
}

void MainWindow::actionClose()
{
    tree_model_->clear();
    file_info_view_->clear();
}

void MainWindow::actionQuit()
{
    QFile file("settings.txt");
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << last_open_path_ << endl;
    }
    file.close();

    spdlog::debug("Shutting down");

    QApplication::quit();
}

void MainWindow::load(const QString &path)
{
    if (path.isEmpty())
        return;

    actionClose();

    last_open_path_ = QFileInfo(path).dir().absolutePath();

    int num_loaded = tree_model_->load(path, vfs_.get());

    // If there is only one top-level archive, expand it.
    // Don't do this for multiple top-level archives because it's messy.
    // Let the user expand them on their own.
    if (num_loaded == 1) {
        tree_view_->expandToDepth(0);
    }

    tree_view_->resizeColumnToContents(0);
    tree_view_->setSortingEnabled(true);
}
