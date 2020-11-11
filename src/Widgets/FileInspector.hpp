/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef FILEINSPECTOR_HPP
#define FILEINSPECTOR_HPP

#include <QFrame>
#include <QItemSelection>

class QPushButton;
class QLineEdit;

class Viewer;
class TreeEntry;

class FileInspector : public QFrame {
    Q_OBJECT
public:
    FileInspector(QWidget *parent = nullptr);

    void addViewer(QString ext, Viewer *viewer);
    void clear();

public slots:
    void setItem(const TreeEntry *const entry);

private slots:
    void saveButtonClicked();
    void viewButtonClicked();

private:
    QLineEdit *mFilename;
    QLineEdit *mSize;
    QLineEdit *mType;

    QPushButton *mSave;
    QPushButton *mView;

    QMap<QString, Viewer *> mViewers;

    const TreeEntry *mItem {nullptr};
};

#endif    // FILEINSPECTOR_HPP
