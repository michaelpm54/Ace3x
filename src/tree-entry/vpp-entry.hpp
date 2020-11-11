/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_VPP_ENTRY_HPP_
#define ACE3X_TREE_ENTRY_VPP_ENTRY_HPP_

#include "Formats/Vpp.hpp"
#include "tree-entry/tree-entry.hpp"

class VppEntry : public TreeEntry {
public:
    VppEntry(QString name);

    void readVpp1(std::vector<std::uint8_t> &data, QTextEdit *log = nullptr);
    void readVpp2(std::vector<std::uint8_t> &data, QTextEdit *log = nullptr);
    void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;
    bool isCompressed() const;

    bool is_archive() const override;

private:
    bool mCompressed {false};
};

#endif    // ACE3X_TREE_ENTRY_VPP_ENTRY_HPP_
