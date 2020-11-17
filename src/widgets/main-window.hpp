/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_MAIN_WINDOW_HPP_
#define ACE3X_WIDGETS_MAIN_WINDOW_HPP_

#include <QItemSelection>
#include <QMainWindow>

class QTreeView;
class QPlainTextEdit;

class TreeModel;
class TreeEntrySortProxy;

class Vfs;

class Ui_MainWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void load(const QString &path);
    QPlainTextEdit *get_log();

public slots:
    void action_open();
    void action_close();
    void action_quit();
    void action_about();

private slots:
    void update_selection(const QItemSelection &selected, const QItemSelection &deselected);
    void load_extra(const QString &path);

private:
    void load_settings();

private:
    Ui_MainWindow *ui;
    std::unique_ptr<Vfs> vfs_;
    TreeModel *tree_model_ {nullptr};
    TreeEntrySortProxy *tree_sort_proxy_ {nullptr};
    QString last_open_path_;
};

#endif    // ACE3X_WIDGETS_MAIN_WINDOW_HPP_
