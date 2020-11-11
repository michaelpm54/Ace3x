/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef PEGENTRY_HPP
#define PEGENTRY_HPP

#include "Entry.hpp"
#include "Formats/Peg.hpp"

class PegEntry : public Entry {
public:
    using Entry::Entry;    // inherit constructors

    void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;
    PegFrame getFrame(std::uint16_t index) const;

private:
    std::vector<PegFrame> mFrames;
};

#endif    // PEGENTRY_HPP
