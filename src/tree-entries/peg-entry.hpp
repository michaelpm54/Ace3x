/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRIES_PEG_ENTRY_HPP_
#define ACE3X_TREE_ENTRIES_PEG_ENTRY_HPP_

#include "format-readers/peg.hpp"
#include "formats/peg.hpp"
#include "tree-entries/tree-entry.hpp"

class PegEntry : public TreeEntry {
public:
    PegEntry(const FileInfo &info);

    Peg::Image getImage(std::uint16_t index) const;

private:
    Peg peg_;
};

#endif    // ACE3X_TREE_ENTRIES_PEG_ENTRY_HPP_
