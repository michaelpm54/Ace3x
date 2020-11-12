/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
#define ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_

#include <QFrame>
#include <QItemSelection>

class QPushButton;
class QLineEdit;

class Viewer;
class TreeEntry;

class FileInfoFrame : public QFrame {
    Q_OBJECT
public:
    FileInfoFrame(QWidget *parent = nullptr);

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

#endif    // ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
