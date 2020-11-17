/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
#define ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_

#include <QFrame>
#include <QItemSelection>

class QPushButton;
class QLineEdit;

class Viewer;
struct VfsEntry;

class Ui_FileInfoFrame;

class FileInfoFrame : public QWidget {
    Q_OBJECT
public:
    FileInfoFrame(QWidget *parent = nullptr);

    void enable_view();
    void clear();

signals:
    void view_clicked(VfsEntry *entry);

public slots:
    void
    set_item(VfsEntry *item);

private slots:
    void save_btn_clicked();
    void view_btn_clicked();

private:
    Ui_FileInfoFrame *ui;
    VfsEntry *item_ {nullptr};
};

#endif    // ACE3X_WIDGETS_FILE_INFO_FRAME_HPP_
