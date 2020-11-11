/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef PEGENTRY_HPP
#define PEGENTRY_HPP

#include "Formats/Peg.hpp"
#include "tree-entry/tree-entry.hpp"

class PegEntry : public TreeEntry {
public:
    using TreeEntry::TreeEntry;    // inherit constructors

    void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;
    PegFrame getFrame(std::uint16_t index) const;

private:
    std::vector<PegFrame> mFrames;
};

#endif    // PEGENTRY_HPP
