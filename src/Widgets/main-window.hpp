/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_MAIN_WINDOW_HPP_
#define ACE3X_WIDGETS_MAIN_WINDOW_HPP_

#include <QItemSelection>
#include <QMainWindow>

class QTreeView;
class QTextEdit;

class TreeModel;
class TreeEntrySortProxy;

class FileInfoFrame;
class ImageViewer;
class PlaintextViewer;
class P3DViewer;
class VIMViewer;
class InfoPanel;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void load(const QString &path);

public slots:
    void actionOpen();
    void actionClose();
    void actionQuit();

private:
    void setupActions();

private slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QTreeView *tree_view_ {nullptr};
    TreeModel *tree_model_ {nullptr};
    TreeEntrySortProxy *tree_sort_proxy_ {nullptr};

    FileInfoFrame *file_info_view_ {nullptr};

    QTextEdit *log_;
    ImageViewer *image_viewer_ {nullptr};
    PlaintextViewer *plaintext_viewer_ {nullptr};
    P3DViewer *p3d_viewer_ {nullptr};
    VIMViewer *vim_viewer_ {nullptr};

    QString last_open_path_;
};

#endif    // ACE3X_WIDGETS_MAIN_WINDOW_HPP_
