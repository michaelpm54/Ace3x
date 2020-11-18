/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_VIEW_MANAGER_HPP_
#define ACE3X_WIDGETS_VIEW_MANAGER_HPP_

#include <QGroupBox>
#include <unordered_map>

class EmptyViewer;
struct VfsEntry;
class Viewer;
class QStackedWidget;

class ViewManager : public QGroupBox {
    Q_OBJECT

public:
    ViewManager(QWidget *parent = nullptr);

    void add_viewer(const std::string &ext, Viewer *viewer);
    bool has_viewer(const std::string &ext) const;
    void clear();

public slots:
    void activate_viewer(VfsEntry *entry);

signals:
    void referenced_file(const std::string &filename);

private:
    QStackedWidget *stack_;
    Viewer *empty_viewer_;
    std::unordered_map<std::string, Viewer *> viewers_;
};

#endif    // ACE3X_WIDGETS_VIEW_MANAGER_HPP_
