/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_
#define ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_

#include <QString>
#include <iostream>
#include <memory>
#include <vector>

class QTextEdit;

struct FileInfo {
    int index_in_parent;
    std::string file_name;
    std::string absolute_path;
    std::string extension;
    // FIXME: This uses more memory than necessary
    std::vector<std::uint8_t> file_data;
};

class TreeEntry {
public:
    TreeEntry();
    TreeEntry(const FileInfo &info);

    virtual ~TreeEntry();
    void removeAllChildren();
    void addChild(TreeEntry *entry);

    unsigned int get_num_children() const;
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
    FileInfo mFileInfo {};

    TreeEntry *mParent {nullptr};
    std::vector<std::unique_ptr<TreeEntry> > mChildren;
    std::vector<std::uint8_t> mData;
};

#endif    // ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_
