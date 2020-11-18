#include "widgets/view-manager.hpp"

#include "vfs/vfs-entry.hpp"
#include "widgets/format-viewers/empty-viewer.hpp"
#include "widgets/format-viewers/viewer.hpp"

ViewManager::ViewManager(QWidget* parent)
    : QStackedWidget(parent)
    , empty_viewer_(new EmptyViewer())
{
    addWidget(empty_viewer_);
}

void ViewManager::add_viewer(const std::string& ext, Viewer* viewer)
{
    assert(!viewers_.count(ext));
    viewers_[ext] = viewer;
    if (indexOf(viewer) == -1) {
        connect(viewer, &Viewer::referenced_file, this, [this](const std::string& filename) {
            emit referenced_file(filename);
        });
        addWidget(viewer);
    }
}

bool ViewManager::has_viewer(const std::string& ext) const
{
    return viewers_.count(ext);
}

void ViewManager::clear()
{
    setCurrentWidget(empty_viewer_);
}

void ViewManager::activate_viewer(VfsEntry* entry)
{
    assert(viewers_.count(entry->extension));
    viewers_[entry->extension]->activate(entry);
    setCurrentWidget(viewers_[entry->extension]);
}
