/* SPDX-License-Identifier: GPLv3-or-later */

#include "tree-entry/vpp-entry.hpp"

#include <zlib.h>

#include <QDebug>
#include <QTextEdit>

#include "Util.hpp"
#include "format-readers/validation-error.hpp"
#include "format-readers/vpp-v1.hpp"
#include "format-readers/vpp-v2.hpp"
#include "tree-entry/peg-entry.hpp"

VppEntry::VppEntry(const FileInfo &vppInfo)
    : TreeEntry(vppInfo)
{
    std::uint32_t version = 0;
    memcpy(&version, &vppInfo.file_data.data()[0x4], 4);

    if (version == 1) {
        VppV1 vpp;
        vpp.read(vppInfo.file_data);

        for (const auto &entryInfo : vpp.get_entries()) {
            if (entryInfo.extension == "peg")
                addChild(new PegEntry(entryInfo));
            else
                addChild(new TreeEntry(entryInfo));
        }
    }
    else if (version == 2) {
        VppV2 vpp;
        vpp.read(vppInfo.file_data);

        mCompressed = vpp.is_compressed();

        for (const auto &entryInfo : vpp.get_entries()) {
            if (entryInfo.extension == "peg")
                addChild(new PegEntry(entryInfo));
            else
                addChild(new TreeEntry(entryInfo));
        }
    }
    else {
        // FIXME: Log
    }
}
