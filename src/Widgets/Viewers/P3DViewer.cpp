/* SPDX-License-Identifier: GPLv3-or-later */

#include "P3DViewer.hpp"

#include "Entry.hpp"

#include "Formats/P3D.hpp"

P3DViewer::P3DViewer(QWidget *parent)
        : QWidget(parent)
{
        ui.setupUi(this);

        resize(800, 600);

        setWindowTitle("Ace3x - P3D Viewer");

        ui.navTable->setColumnCount(8);
        ui.navTable->setHorizontalHeaderLabels({ "Name", "?", "?", "?", "?", "x", "y", "z" });
        ui.navTable->setSortingEnabled(true);
}

void P3DViewer::activate(const Entry *item)
{
        ui.objList->clear();
        ui.imgList->clear();
        ui.lyrList->clear();
        ui.navTable->clearContents();

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
                        std::string str(reinterpret_cast<const char *>(&ptr[offset]));
                        if (!str.empty())
                                names.push_back(str);
                        offset += names.back().size() + 1;
                } while (offset < namesEnd);
        }

        for (auto &f : names) {
                auto str = QString::fromStdString(f);
                if (str.endsWith("tga", Qt::CaseSensitivity::CaseInsensitive))
                        ui.imgList->addItem(str);
                else if (str.startsWith('$')) {
                        // $player, $npc, $hostile
                        if (str.startsWith("$p") || str.startsWith("$n") || str.startsWith("$h"))
                                addNavpoint(str, header, ptr);
                        else
                                ui.lyrList->addItem(str);
                } else
                        ui.objList->addItem(str);

        }

        ui.objCount->setNum(ui.objList->count());
        ui.imgCount->setNum(ui.imgList->count());
        ui.lyrCount->setNum(ui.lyrList->count());
        ui.navCount->setNum(ui.navTable->rowCount());

        ui.navTable->resizeColumnsToContents();
        ui.navTable->sortItems(0, Qt::SortOrder::AscendingOrder);
}

bool P3DViewer::shouldBeEnabled(const Entry *) const
{
        return true;
}

void P3DViewer::addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *data)
{
        const int row = ui.navTable->rowCount();

        Navpoint nav;
        memcpy(&nav, &data[header.navpointsStart + (row * sizeof(Navpoint))], sizeof(Navpoint));

        ui.navTable->insertRow(row);

        ui.navTable->setItem(row, 0, new QTableWidgetItem(str));
        ui.navTable->setItem(row, 1, new QTableWidgetItem(QString::number(*reinterpret_cast<int *>(&nav.unk0))));
        ui.navTable->setItem(row, 2, new QTableWidgetItem(QString::number(nav.unk4, 'g', 4)));
        ui.navTable->setItem(row, 3, new QTableWidgetItem(QString::number(nav.unk14, 'g', 4)));
        ui.navTable->setItem(row, 4, new QTableWidgetItem(QString::number(nav.unk24, 'g', 4)));
        ui.navTable->setItem(row, 5, new QTableWidgetItem(QString::number(nav.x, 'g', 4)));
        ui.navTable->setItem(row, 6, new QTableWidgetItem(QString::number(nav.y, 'g', 4)));
        ui.navTable->setItem(row, 7, new QTableWidgetItem(QString::number(nav.z, 'g', 4)));
}
