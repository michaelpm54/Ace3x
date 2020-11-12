/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-model.hpp"

#include <QLocale>
#include <filesystem>
#include <regex>

#include "file-info.hpp"
#include "fs.hpp"
#include "tree-entries/tree-entry.hpp"
#include "tree-entries/vpp-entry.hpp"

TreeModel::TreeModel()
    : invisible_root_(new TreeEntry())
{
    clear();
}

TreeModel::~TreeModel() = default;

TreeEntry *TreeModel::itemFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return invisible_root_.get();

    return static_cast<TreeEntry *>(index.internalPointer());
}

QModelIndex TreeModel::index(int row, int col, const QModelIndex &parent) const
{
    if (!hasIndex(row, col, parent))
        return QModelIndex();

    TreeEntry *parent_entry;

    if (!parent.isValid())
        parent_entry = invisible_root_.get();
    else
        parent_entry = static_cast<TreeEntry *>(parent.internalPointer());

    // Create a new index for this item
    TreeEntry *childItem = parent_entry->getChild(row);
    if (childItem)
        return createIndex(row, col, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeEntry *child_entry = static_cast<TreeEntry *>(index.internalPointer());
    TreeEntry *parent_entry = child_entry->getParent();

    // Return an invalid index for the root as it has no parent
    if (parent_entry == invisible_root_.get())
        return QModelIndex();

    // Create the parent index
    return createIndex(parent_entry->getIndex(), 0, parent_entry);
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeEntry *item = itemFromIndex(index);

    if (role == Qt::DisplayRole)
        switch (index.column()) {
            case 0:
                return QString::fromStdString(item->getFilename());
            case 1:
                return QLocale::system().formattedDataSize(item->getSize(), 2, nullptr);
            default:
                break;
        }

    return QVariant();
}

bool TreeModel::hasChildren(const QModelIndex &index) const
{
    if (!index.isValid())
        return true;

    const auto *item = itemFromIndex(index);
    return item->get_num_children() != 0;
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

    TreeEntry *parent_entry;
    if (!parent.isValid())
        parent_entry = invisible_root_.get();
    else
        parent_entry = static_cast<TreeEntry *>(parent.internalPointer());

    return parent_entry->get_num_children();
}

int TreeModel::columnCount(const QModelIndex &) const
{
    return 2;
}

void TreeModel::clear()
{
    beginResetModel();

    invisible_root_->removeAllChildren();

    endResetModel();
}

bool TreeModel::canFetchMore(const QModelIndex &index) const
{
    if (!index.isValid())
        return false;

    return itemFromIndex(index)->get_num_children() > 0;
}

void TreeModel::fetchMore(const QModelIndex &)
{
    // FIXME
    return;
}

void TreeModel::addTopLevelEntry(TreeEntry *entry)
{
    beginInsertRows(QModelIndex(), invisible_root_->get_num_children(), invisible_root_->get_num_children());

    invisible_root_->addChild(entry);

    endInsertRows();
}

int TreeModel::load(const QString &path)
{
    if (path.isEmpty()) {
        return 0;
    }

    int num_loaded = 0;

    if (path.contains("LEVELS")) {
        const auto level_vpps = TreeModel::gather_level_vpps_in_dir(path.toStdString(), true);
        for (const auto &vpp : level_vpps) {
            addTopLevelEntry(new VppEntry(vpp));
        }
        num_loaded = level_vpps.size();
    }
    else {
        const auto fs_path = std::filesystem::path(path.toStdString());

        FileInfo info;
        info.index_in_parent = 0;
        info.file_name = fs_path.filename().string();
        info.extension = fs_path.extension().string();
        info.absolute_path = std::filesystem::absolute(fs_path).string();

        try {
            info.file_data = ace3x::fs::load_file_to_vector(path.toStdString());
            addTopLevelEntry(new VppEntry(info));
        }
        catch (const std::runtime_error &e) {
            // FIXME: Log
            // log_->append(QString("[Error] Failed to load VPP: %1").arg(e.what()));
        }
        catch (...) {
            // log_->append(QString("[Error] Failed to load VPP: Unhandled exception type"));
        }

        num_loaded = 1;
    }

    return num_loaded;
}

std::vector<FileInfo> TreeModel::gather_level_vpps_in_dir(const std::string &dir, bool load_data)
{
    std::vector<FileInfo> vpps;

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

    const std::string dir_ = std::filesystem::is_directory(dir) ? dir : std::filesystem::path(dir).parent_path().string();

    int index = 0;
    for (const auto &entry : std::filesystem::directory_iterator(dir_)) {
        if (!std::regex_search(entry.path().filename().string(), level_regex)) {
            continue;
        }

        FileInfo info;
        info.index_in_parent = index++;
        info.file_name = entry.path().filename().string();
        info.absolute_path = std::filesystem::absolute(entry.path()).string();
        info.file_data = ace3x::fs::load_file_to_vector(info.absolute_path);

        vpps.push_back(info);
    }

    return vpps;
}
