/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
#define ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_

#include <QFrame>
#include <QItemSelection>

class QPushButton;
class QLineEdit;

class Viewer;
struct VfsEntry;

class FileInfoFrame : public QFrame {
    Q_OBJECT
public:
    FileInfoFrame(QWidget *parent = nullptr);

    void addViewer(QString ext, Viewer *viewer);
    void clear();

public slots:
    void setItem(VfsEntry *item);

signals:
    void viewClicked(VfsEntry *entry);

private slots:
    void saveButtonClicked();
    void viewButtonClicked();

private:
    QLineEdit *filename_;
    QLineEdit *size_;
    QLineEdit *type_;

    QPushButton *save_;
    QPushButton *view_;

    QMap<QString, Viewer *> viewers_;

    VfsEntry *item_ {nullptr};
};

#endif    // ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
