/* SPDX-License-Identifier: GPLv3-or-later */

#include "Widgets/main-window.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextEdit>
#include <QTextStream>
#include <QTreeView>

#include "tree-model/sort-proxy.hpp"
#include "tree-model/tree-model.hpp"
#include "ui_main-window.h"
#include "vfs/mmap-vfs.hpp"
#include "widgets/file-info-frame.hpp"
#include "widgets/format-viewers/image-viewer.hpp"
#include "widgets/format-viewers/p3d-viewer.hpp"
#include "widgets/format-viewers/plaintext-viewer.hpp"
#include "widgets/format-viewers/vf2-viewer.hpp"
#include "widgets/format-viewers/vim-viewer.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui_MainWindow())
    , vfs_(std::make_unique<MmapVfs>())
    , tree_model_(new TreeModel(vfs_.get()))
    , tree_sort_proxy_(new TreeEntrySortProxy())
    , last_open_path_(QDir::currentPath())
{
    ui->setupUi(this);

    ui->action_open->setShortcut({"Ctrl+O"});
    ui->action_close->setShortcut({"Ctrl+W"});
    ui->action_quit->setShortcut({"Ctrl+Q"});

    tree_sort_proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    tree_sort_proxy_->setSourceModel(tree_model_);
    ui->tree_view->setModel(tree_sort_proxy_);

    auto *image_viewer {new ImageViewer()};
    auto *text_viewer {new PlaintextViewer()};
    auto *vf2_viewer {new Vf2Viewer(vfs_.get())};
    ui->view_stack->add_viewer(".peg", image_viewer);
    ui->view_stack->add_viewer(".tga", image_viewer);
    ui->view_stack->add_viewer(".vbm", image_viewer);
    ui->view_stack->add_viewer(".tbl", text_viewer);
    ui->view_stack->add_viewer(".arr", text_viewer);
    ui->view_stack->add_viewer(".vim", new VIMViewer());
    ui->view_stack->add_viewer(".p3d", new P3DViewer());
    ui->view_stack->add_viewer(".vf2", vf2_viewer);

    load_settings();

    connect(ui->action_open, &QAction::triggered, this, &MainWindow::action_open);
    connect(ui->action_close, &QAction::triggered, this, &MainWindow::action_close);
    connect(ui->action_quit, &QAction::triggered, this, &MainWindow::action_quit);
    connect(ui->action_about, &QAction::triggered, this, &MainWindow::action_about);

    connect(ui->tree_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::update_selection);
    connect(ui->tree_view, &QTreeView::expanded, this, [this]() {
        ui->tree_view->resizeColumnToContents(0);
    });
    connect(ui->inspector, &FileInfoFrame::view_clicked, this, [this]() {
        ui->referenced_files->clear();
    });
    connect(ui->inspector, &FileInfoFrame::view_clicked, ui->view_stack, &ViewManager::activate_viewer);
    connect(ui->clear_log_btn, &QPushButton::clicked, this, [this]() {
        ui->log->clear();
    });
    connect(vf2_viewer, &Vf2Viewer::request_load, this, &MainWindow::load_extra);
    connect(ui->view_stack, &ViewManager::referenced_file, this, &MainWindow::add_referenced_file);
}

MainWindow::~MainWindow()
{
    delete tree_model_;
    delete tree_sort_proxy_;
    delete ui;
}

QPlainTextEdit *MainWindow::get_log()
{
    return ui->log;
}

void MainWindow::update_selection(const QItemSelection &selected, const QItemSelection &)
{
    if (selected.isEmpty()) {
        return;
    }

    auto *entry = tree_model_->itemFromIndex(tree_sort_proxy_->mapToSource(selected.indexes().first()));

    ui->inspector->set_item(entry);

    if (ui->view_stack->has_viewer(entry->extension)) {
        ui->inspector->enable_view();
    }
}

void MainWindow::action_open()
{
    load(QFileDialog::getOpenFileName(this, "Open VPP", last_open_path_, "Archive File (*.VPP)"));
}

void MainWindow::action_close()
{
    tree_model_->clear();
    ui->inspector->clear();
    ui->view_stack->clear();
    vfs_->clear();
    ui->action_close->setEnabled(false);
}

void MainWindow::action_quit()
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

    action_close();

    last_open_path_ = QFileInfo(path).dir().absolutePath();

    int num_loaded = tree_model_->load(path, vfs_.get());

    /* If there is only one top-level archive, expand it.
     * Don't do this for multiple top-level archives because it's messy.
     * Let the user expand them on their own.
     */
    if (num_loaded == 1) {
        ui->tree_view->expandToDepth(0);
    }

    ui->tree_view->resizeColumnToContents(0);

    ui->action_close->setEnabled(true);
}

void MainWindow::load_extra(const QString &path)
{
    if (path.isEmpty())
        return;

    tree_model_->load(path, vfs_.get());

    ui->tree_view->resizeColumnToContents(0);

    ui->action_close->setEnabled(true);
}

void MainWindow::load_settings()
{
    if (std::filesystem::exists("settings.txt")) {
        QFile file("settings.txt");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            stream >> last_open_path_;
            spdlog::info("Loaded settings.txt");
            spdlog::info("Recently opened path: {}", last_open_path_.toStdString());
        }
        file.close();
    }
    else {
        spdlog::info("settings.txt not found");
    }
}

void MainWindow::action_about()
{
    /* clang-format off */
    QMessageBox::about(this, "Ace3x - About",
                       "Ace3x resource viewer for Summoner 2.<br/>"
                       "License - <a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">GNU GENERAL PUBLIC LICENSE Version 3</a><br/>"
                       "<a href=\"https://github.com/michaelpm54/Ace3x\">GitHub</a>");
    /* clang-format on */
}

void MainWindow::add_referenced_file(const std::string &filename)
{
    ui->referenced_files->addItem(new QListWidgetItem(QString::fromStdString(filename)));
}
