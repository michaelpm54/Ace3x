/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_MAIN_WINDOW_HPP_
#define ACE3X_WIDGETS_MAIN_WINDOW_HPP_

#include <QItemSelection>
#include <QMainWindow>

#include "tree-entries/tree-entry.hpp"

class QFrame;
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
    QTreeView *mFileView {nullptr};
    TreeModel *mFileViewModel {nullptr};
    TreeEntrySortProxy *mTreeEntrySortProxy {nullptr};

    FileInfoFrame *mFileInspector {nullptr};

    QTextEdit *mLog;
    ImageViewer *mImageViewer {nullptr};
    PlaintextViewer *mPlaintextViewer {nullptr};
    P3DViewer *mP3DViewer {nullptr};
    VIMViewer *mVIMViewer {nullptr};

    QString mLastPath;
};

#endif    // ACE3X_WIDGETS_MAIN_WINDOW_HPP_
