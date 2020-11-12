/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entries/peg-entry.hpp"

#include <QDebug>
#include <QTextEdit>
#include <sstream>

#include "format-readers/peg.hpp"

PegEntry::PegEntry(const FileInfo &pegInfo)
    : TreeEntry(pegInfo)
{
    peg_.read(pegInfo.file_data);

    for (const auto &entryInfo : peg_.get_entries()) {
        addChild(new TreeEntry(entryInfo));
    }
}

Peg::Image PegEntry::getImage(std::uint16_t index) const
{
    return peg_.getImage(index);
}
