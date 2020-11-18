#include "widgets/view-manager.hpp"

#include <QStackedWidget>
#include <QVBoxLayout>

#include "vfs/vfs-entry.hpp"
#include "widgets/format-viewers/empty-viewer.hpp"
#include "widgets/format-viewers/viewer.hpp"

ViewManager::ViewManager(QWidget* parent)
    : QGroupBox("No viewer", parent)
    , stack_(new QStackedWidget())
    , empty_viewer_(new EmptyViewer())
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(stack_);
    stack_->addWidget(empty_viewer_);
}

void ViewManager::add_viewer(const std::string& ext, Viewer* viewer)
{
    assert(!viewers_.count(ext));
    viewers_[ext] = viewer;
    if (stack_->indexOf(viewer) == -1) {
        connect(viewer, &Viewer::referenced_file, this, [this](const std::string& filename) {
            emit referenced_file(filename);
        });
        stack_->addWidget(viewer);
    }
}

bool ViewManager::has_viewer(const std::string& ext) const
{
    return viewers_.count(ext);
}

void ViewManager::clear()
{
    stack_->setCurrentWidget(empty_viewer_);
    setTitle("No viewer");
}

void ViewManager::activate_viewer(VfsEntry* entry)
{
    assert(viewers_.count(entry->extension));
    viewers_[entry->extension]->activate(entry);
    stack_->setCurrentWidget(viewers_[entry->extension]);
    setTitle(QString::fromStdString(entry->extension));
}
