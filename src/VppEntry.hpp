/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VPPENTRY_HPP
#define VPPENTRY_HPP

#include "Formats/Vpp.hpp"
#include "tree-entry/tree-entry.hpp"


class VppEntry : public TreeEntry {
public:
    VppEntry(QString name);

    void readVpp1(std::vector<std::uint8_t> &data, QTextEdit *log = nullptr);
    void readVpp2(std::vector<std::uint8_t> &data, QTextEdit *log = nullptr);
    void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;
    bool isCompressed() const;

private:
    bool mCompressed {false};
};

#endif    // VPPENTRY_HPP
