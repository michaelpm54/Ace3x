/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRIES_VPP_ENTRY_HPP_
#define ACE3X_TREE_ENTRIES_VPP_ENTRY_HPP_

#include "tree-entries/tree-entry.hpp"

class VppEntry : public TreeEntry {
public:
    VppEntry(const FileInfo &info);
    bool isCompressed() const;

private:
    bool mCompressed {false};
};

#endif    // ACE3X_TREE_ENTRIES_VPP_ENTRY_HPP_
