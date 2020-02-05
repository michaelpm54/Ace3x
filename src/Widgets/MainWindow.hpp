/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QItemSelection>

#include "Entry.hpp"

class QFrame;
class QTreeView;
class QTextEdit;

class FileViewModel;
class SortProxy;

class FileInspector;
class ImageViewer;
class PlaintextViewer;
class P3DViewer;
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
        void           setupActions();
        QList<Entry *> loadLevel(const QString &path);

private slots:
        void updateSelection(const QItemSelection &selected, const QItemSelection &deselected);

private:
	QTreeView *mFileView{ nullptr };
	FileViewModel *mFileViewModel{ nullptr };
        SortProxy *mProxyModel{ nullptr };

	FileInspector *mFileInspector{ nullptr };

        QTextEdit *mLog;
	ImageViewer *mImageViewer{ nullptr };
        PlaintextViewer *mPlaintextViewer{ nullptr };
        P3DViewer *mP3DViewer{ nullptr };

        QString mLastPath;
};

#endif // MAIN_WINDOW_H
