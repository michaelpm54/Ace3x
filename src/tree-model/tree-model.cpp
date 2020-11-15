/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-model/tree-model.hpp"

#include <spdlog/spdlog.h>

#include <QLocale>
#include <filesystem>
#include <regex>

#include "vfs/vfs-entry.hpp"
#include "vfs/vfs.hpp"

TreeModel::TreeModel(Vfs *vfs)
    : vfs_(vfs)
{
}

TreeModel::~TreeModel() = default;

VfsEntry *TreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;

    return static_cast<VfsEntry *>(index.internalPointer());
}

QModelIndex TreeModel::index(int row, int col, const QModelIndex &parent) const
{
    if (!hasIndex(row, col, parent))
        return QModelIndex();

    if (!parent.isValid()) {
        return createIndex(row, col, invisible_root_[row]);
    }

    auto *parent_entry = static_cast<VfsEntry *>(parent.internalPointer());

    if (row >= parent_entry->entries.size())
        return QModelIndex();

    return createIndex(row, col, parent_entry->entries[row]);
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    VfsEntry *child_entry = static_cast<VfsEntry *>(index.internalPointer());
    VfsEntry *parent_entry = child_entry->parent;

    // Return an invalid index for the root as it has no parent
    if (!parent_entry)
        return QModelIndex();

    // Create the parent index
    return createIndex(parent_entry->index, 0, parent_entry);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const VfsEntry *item = itemFromIndex(index);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return QString::fromStdString(item->name);
            case 1:
                return QLocale::system().formattedDataSize(item->size, 2, nullptr);
            default:
                break;
        }
    }

    return QVariant();
}

bool TreeModel::hasChildren(const QModelIndex &index) const
{
    if (!index.isValid())
        return true;

    auto *entry = itemFromIndex(index);

    return entry->entries.size() || entry->extension == ".peg";
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole))
        switch (section) {
            case 0:
                return "Filename";
            case 1:
                return "Size";
            default:
                break;
        }

    return QVariant();
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 1) {
        return 0;
    }

    const VfsEntry *parent_entry;
    if (!parent.isValid())
        return invisible_root_.size();
    else
        parent_entry = static_cast<const VfsEntry *>(parent.internalPointer());

    return parent_entry->entries.size();
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return 2;
}

void TreeModel::clear()
{
    beginResetModel();

    invisible_root_.clear();

    endResetModel();
}

void TreeModel::addTopLevelEntry(VfsEntry *entry)
{
    beginInsertRows(QModelIndex(), invisible_root_.size(), invisible_root_.size());

    entry->index = invisible_root_.size();
    invisible_root_.push_back(entry);

    endInsertRows();

    spdlog::info("Tree: Loaded VPP: {}", entry->name);
}

int TreeModel::load(const QString &path, Vfs *vfs)
{
    if (path.isEmpty()) {
        return 0;
    }

    int num_loaded = 0;

    if (path.contains("LEVELS")) {
        /* Examples:
		    HA_SRC00.VPP
		    HA_SRC01.VPP
		    HA_SRC02.VPP
		    HA_SRC03.VPP
		    HA_SRC04.VPP
		    HA_CORE1.VPP
		    HA_CORE2.VPP
	    */
        std::regex level_regex("(SRC\\d\\d|CORE\\d).VPP", std::regex_constants::icase);

        const std::string dir = std::filesystem::is_directory(path.toStdString()) ? path.toStdString() : std::filesystem::path(path.toStdString()).parent_path().string();

        for (const auto &entry : std::filesystem::directory_iterator(dir)) {
            if (!std::regex_search(entry.path().filename().string(), level_regex)) {
                continue;
            }
            const auto path = entry.path().generic_string();
            if (vfs->add_root_archive(path)) {
                addTopLevelEntry(vfs->get_entry(path));
                num_loaded++;
            }
        }
    }
    else {
        const auto fs_path = std::filesystem::path(path.toStdString());
        if (vfs->add_root_archive(fs_path.string())) {
            auto *vpp = vfs->get_entry(fs_path.string());
            addTopLevelEntry(vpp);
            num_loaded = 1;
        }
    }

    return num_loaded;
}
