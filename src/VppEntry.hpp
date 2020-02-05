/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef VPPENTRY_HPP
#define VPPENTRY_HPP

#include "Entry.hpp"
#include "Formats/Vpp.hpp"

class VppEntry : public Entry {
public:
        VppEntry(QString name);

        void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr) override;
        bool isCompressed() const;

private:
        bool mCompressed { false };
};

#endif // VPPENTRY_HPP
