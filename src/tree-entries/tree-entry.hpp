/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRIES_TREE_ENTRY_HPP_
#define ACE3X_TREE_ENTRIES_TREE_ENTRY_HPP_

#include <QString>
#include <iostream>
#include <memory>
#include <vector>

#include "file-info.hpp"

class TreeEntry {
public:
    TreeEntry();
    TreeEntry(const FileInfo &info);

    virtual ~TreeEntry();
    void removeAllChildren();
    void addChild(TreeEntry *entry);

    int get_num_children() const;
    int getIndex() const;
    std::string getName() const;
    std::string getPath() const;
    std::string getFilename() const;
    std::string getExtension() const;
    std::uint64_t getSize() const;
    const std::uint8_t *getData() const;
    TreeEntry *getParent() const;
    TreeEntry *getChild(std::uint16_t index) const;

protected:
    FileInfo file_info_ {};
    TreeEntry *parent_ {nullptr};
    std::vector<std::unique_ptr<TreeEntry> > children_;
};

#endif    // ACE3X_TREE_ENTRIES_TREE_ENTRY_HPP_
