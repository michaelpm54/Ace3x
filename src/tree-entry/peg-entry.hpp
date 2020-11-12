/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_PEG_ENTRY_HPP_
#define ACE3X_TREE_ENTRY_PEG_ENTRY_HPP_

#include "formats/Peg.hpp"
#include "tree-entry/tree-entry.hpp"

class PegEntry : public TreeEntry {
public:
    PegEntry(const FileInfo &info);

    PegFrame getFrame(std::uint16_t index) const;

private:
    std::vector<PegFrame> mFrames;
};

#endif    // ACE3X_TREE_ENTRY_PEG_ENTRY_HPP_
