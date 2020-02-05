/* SPDX-License-Identifier: GPLv3-or-later */

#include "P3DViewer.hpp"

#include "Entry.hpp"

P3DViewer::P3DViewer(QWidget *parent)
        : QWidget(parent)
{
        ui.setupUi(this);

        resize(800, 600);

        setWindowTitle("Ace3x - P3D Viewer");
}

struct P3DHeader {
        std::uint32_t signature;
        std::uint32_t version;
        std::uint32_t size;
        std::uint32_t unkC;
        std::uint32_t unk10;
        std::uint32_t unk14;
        std::uint32_t dataStart;
        std::uint32_t unk1C;
        std::uint32_t unk20;
        std::uint32_t unk24;
        std::uint32_t unk28;
        std::uint32_t unk2C;
        std::uint32_t unk30;
        std::uint32_t unk34;
        std::uint32_t unk38;
        std::uint32_t unk3C;
        std::uint32_t unk40;
        std::uint32_t unk44;
        std::uint32_t unk48;
        std::uint32_t unk4C;
        std::uint32_t unk50;
        std::uint32_t unk54;
        std::uint32_t unk58;
};

void P3DViewer::activate(const Entry *item)
{
        ui.objects->clear();
        ui.images->clear();
        ui.markers->clear();

        show();

        auto ptr = item->getData();

        P3DHeader header;
        memcpy(&header, ptr, sizeof(P3DHeader));
        header.size += 0x8;

        std::uint32_t namesStart = sizeof(P3DHeader);
        std::uint32_t namesEnd   = header.dataStart;
        std::vector<std::string> names;
        {
                std::ptrdiff_t offset = namesStart;
                do {
                        names.push_back(std::string(reinterpret_cast<const char *>(&ptr[offset])));
                        offset += names.back().size() + 1;
                } while (offset < namesEnd);
        }

        for (auto &f : names) {
                auto str = QString::fromStdString(f);
                if (str.endsWith("tga", Qt::CaseSensitivity::CaseInsensitive))
                        ui.images->addItem(str);
                else if (str.startsWith('$'))
                        ui.markers->addItem(str);
                else
                        ui.objects->addItem(str);
        }

        ui.objCount->setNum(ui.objects->count());
        ui.imgCount->setNum(ui.images->count());
        ui.mrkCount->setNum(ui.markers->count());
}

bool P3DViewer::shouldBeEnabled(const Entry *) const
{
        return true;
}
