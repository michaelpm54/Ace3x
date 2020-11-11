/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_FILE_ENTRY_HPP_
#define ACE3X_TREE_ENTRY_FILE_ENTRY_HPP_

#include "tree-entry/tree-entry.hpp"

class FileEntry : public TreeEntry {
public:
    using TreeEntry::TreeEntry;    // inherit constructors

    void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;

    bool is_archive() const override;
};

#endif    // ACE3X_TREE_ENTRY_FILE_ENTRY_HPP_
