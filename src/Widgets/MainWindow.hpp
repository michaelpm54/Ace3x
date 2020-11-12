/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QItemSelection>
#include <QMainWindow>

#include "tree-entry/tree-entry.hpp"

class QFrame;
class QTreeView;
class QTextEdit;

class TreeModel;
class TreeEntrySortProxy;

class FileInspector;
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

    void loadVpp(const QString &path);

public slots:
    void actionOpen();
    void actionClose();
    void actionQuit();

private:
    void setupActions();
    QList<TreeEntry *> loadLevelVpps(const QString &path);

private slots:
    void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QTreeView *mFileView {nullptr};
    TreeModel *mFileViewModel {nullptr};
    TreeEntrySortProxy *mTreeEntrySortProxy {nullptr};

    FileInspector *mFileInspector {nullptr};

    QTextEdit *mLog;
    ImageViewer *mImageViewer {nullptr};
    PlaintextViewer *mPlaintextViewer {nullptr};
    P3DViewer *mP3DViewer {nullptr};
    VIMViewer *mVIMViewer {nullptr};

    QString mLastPath;
};

#endif    // MAIN_WINDOW_H
