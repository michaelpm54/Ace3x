/* SPDX-License-Identifier: GPLv3-or-later */

#include "MainWindow.hpp"

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

#include "Util.hpp"
#include "Widgets/FileInspector.hpp"
#include "Widgets/Viewers/ImageViewer.hpp"
#include "Widgets/Viewers/P3DViewer.hpp"
#include "Widgets/Viewers/PlaintextViewer.hpp"
#include "Widgets/Viewers/VIMViewer.hpp"
#include "tree-entry/tree-entry-sort-proxy.hpp"
#include "tree-entry/vpp-entry.hpp"
#include "tree-model.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mFileView(new QTreeView())
    , mFileViewModel(new TreeModel())
    , mTreeEntrySortProxy(new TreeEntrySortProxy())
    , mFileInspector(new FileInspector())
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

    mFileInspector->addViewer("peg", mImageViewer);
    mFileInspector->addViewer("tga", mImageViewer);
    mFileInspector->addViewer("vbm", mImageViewer);

    mFileInspector->addViewer("tbl", mPlaintextViewer);
    mFileInspector->addViewer("arr", mPlaintextViewer);

    mFileInspector->addViewer("p3d", mP3DViewer);

    mFileInspector->addViewer("vim", mVIMViewer);
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

QList<TreeEntry *> MainWindow::loadLevelVpps(const QString &path)
{
    QDir dir = QFileInfo(path).dir();

    auto entryPaths = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    QList<TreeEntry *> list;

    int index = 0;
    for (auto &e : entryPaths) {
        if (!e.fileName().endsWith("VPP", Qt::CaseSensitivity::CaseInsensitive))
            continue;

        FileInfo info;
        info.index_in_parent = index++;
        info.absolute_path = e.absoluteFilePath().toStdString();
        info.file_name = e.fileName().toStdString();

        VppEntry *vpp {nullptr};

        try {
            info.file_data = LoadFile(path.toStdString());
            vpp = new VppEntry(info);
        }
        catch (const std::runtime_error &e) {
            mLog->append(QString("[Error] Failed to load VPP: %1").arg(e.what()));
            continue;
        }
        catch (...) {
            mLog->append(QString("[Error] Failed to load VPP: Unhandled exception type"));
            continue;
        }

        vpp->addChildrenFromData(info.file_data, mLog);
        list.append(vpp);
        mLog->append(QString("[Info] Adding root %1").arg(QString::fromStdString(info.file_name)));
    }

    return list;
}

void MainWindow::loadVpp(const QString &path)
{
    if (path.isEmpty()) {
        std::clog << "VPP path empty" << std::endl;
        return;
    }

    mLastPath = QFileInfo(path).dir().absolutePath();

    QList<TreeEntry *> vppList;

    if (path.contains("LEVELS")) {
        vppList.append(loadLevelVpps(path));
    }
    else {
        FileInfo info;
        info.index_in_parent = 0;
        info.file_name = std::filesystem::path(path.toStdString()).filename().string();
        info.absolute_path = path.toStdString();

        VppEntry *vpp {nullptr};

        try {
            info.file_data = LoadFile(path.toStdString());
            vpp = new VppEntry(info);
        }
        catch (const std::runtime_error &e) {
            mLog->append(QString("[Error] Failed to load VPP: %1").arg(e.what()));
        }
        catch (...) {
            mLog->append(QString("[Error] Failed to load VPP: Unhandled exception type"));
        }

        if (vpp) {
            try {
                vpp->addChildrenFromData(info.file_data, mLog);
                mLog->append(QString("[Info] Adding VPP %1").arg(QFileInfo(path).fileName()));
                vppList.append(vpp);
            }
            catch (const ValidationError &e) {
                std::clog << path.toStdString() << ": " << e.what() << std::endl;
                delete vpp;
            }
        }
    }

    mFileViewModel->addRoots(vppList);
    mFileView->expandToDepth(0);
    mFileView->resizeColumnToContents(0);
    mFileView->setSortingEnabled(true);
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
    const auto fileName = QFileDialog::getOpenFileName(this, "Open VPP", mLastPath, "Archive File (*.VPP)");
    if (!fileName.isEmpty()) {
        loadVpp(fileName);
    }
    else {
        mLog->append("[Info] Open: selection empty");
    }
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
