/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/p3d-viewer.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <array>
#include <fstream>

#include "ui_p3d-viewer.h"
#include "vfs/vfs-entry.hpp"

void P3DViewer::writeVerticesToObj(const QString &fileName, const P3DHeader &header, const std::uint8_t *const vertex_data)
{
    std::vector<P3DSubHeader> subHeaders(header.numSubHeaders);
    memcpy(subHeaders.data(), vertex_data + header.filenamesEnd, sizeof(P3DSubHeader) * header.numSubHeaders);

    const auto filename = fmt::format("{}.obj", fileName.toStdString());

    std::ofstream file(filename, std::ios::trunc);

    if (!file.good()) {
        spdlog::error("P3D: Failed to open file '{}' for writing vertices", filename);
        return;
    }

    struct Face {
        std::vector<std::array<float, 3> > positions;
        std::vector<int> indices;
    };

    using Mesh = std::vector<Face>;
    std::vector<Mesh> meshes(header.numSubHeaders);

    int index = 0;

    for (auto i = 0u; i < 1; i++) {
        const float *ptr = reinterpret_cast<const float *>(&vertex_data[subHeaders[i].offsetC]);

        auto size = *(std::uint32_t *)(&ptr[3]);
        while (size < 100 && size != 0) {
            Face face;
            face.indices.resize(size);
            face.positions.resize(size);
            for (auto j = 0u; j < size; j++) {
                memcpy(face.positions[j].data(), ptr, 12);
                face.indices[j] = index++;
                ptr += 16;
            }
            meshes[i].push_back(face);
            size = *(std::uint32_t *)(&ptr[3]);
        }
    }

    file << "o Object\n";

    for (const auto &mesh : meshes) {
        for (const auto &face : mesh) {
            for (const auto &vertex : face.positions) {
                file << "v " << vertex[0] << ' ' << vertex[1] << ' ' << vertex[2] << '\n';
            }
        }
    }

    file.flush();
    file.close();

    spdlog::info("P3D: Wrote vertices to file '{}'", filename);
}

P3DViewer::P3DViewer(QWidget *parent)
    : Viewer(parent)
    , ui_(new Ui::P3DViewer())
{
    ui_->setupUi(this);

    ui_->navTable->setColumnCount(8);
    ui_->navTable->setHorizontalHeaderLabels({"Name", "?", "?", "?", "?", "x", "y", "z"});
    ui_->navTable->setSortingEnabled(true);

    connect(ui_->writeToObjButton, &QPushButton::clicked, this, &P3DViewer::onWriteObjClicked);
}

void P3DViewer::onWriteObjClicked()
{
    writeVerticesToObj(QString::fromStdString(item_->name), p3d_header_, item_->data);
}

void P3DViewer::activate(const VfsEntry *item)
{
    ui_->objList->clear();
    ui_->imgList->clear();
    ui_->lyrList->clear();
    ui_->navTable->clearContents();

    show();

    item_ = item;

    auto ptr = item->data;

    P3DHeader header;
    memcpy(&header, ptr, sizeof(P3DHeader));
    header.size += 0x8;

    p3d_header_ = header;

    std::uint32_t namesStart = sizeof(P3DHeader);
    std::uint32_t namesEnd = header.filenamesEnd;
    std::vector<std::string> names;
    {
        std::ptrdiff_t offset = namesStart;
        do {
            std::string str(reinterpret_cast<const char *>(&ptr[offset]));
            if (!str.empty()) {
                names.push_back(str);
            }
            offset += names.back().size() + 1;
        } while (offset < namesEnd);
    }

    for (auto &f : names) {
        auto str = QString::fromStdString(f);
        if (str.endsWith("tga", Qt::CaseSensitivity::CaseInsensitive)) {
            emit referenced_file(str.toStdString());
            ui_->imgList->addItem(str);
        }
        else if (str.startsWith('$')) {
            // $player, $npc, $hostile
            if (str.startsWith("$p") || str.startsWith("$n") || str.startsWith("$h")) {
                addNavpoint(str, header, ptr);
            }
            else {
                ui_->lyrList->addItem(str);
            }
        }
        else {
            ui_->objList->addItem(str);
        }
    }

    ui_->objCount->setNum(ui_->objList->count());
    ui_->imgCount->setNum(ui_->imgList->count());
    ui_->lyrCount->setNum(ui_->lyrList->count());
    ui_->navCount->setNum(ui_->navTable->rowCount());

    ui_->navTable->resizeColumnsToContents();
    ui_->navTable->sortItems(0, Qt::SortOrder::AscendingOrder);
}

bool P3DViewer::shouldBeEnabled(const VfsEntry *) const
{
    return (true);
}

void P3DViewer::addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *const navpoint_data)
{
    const int row = ui_->navTable->rowCount();

    Navpoint nav;
    memcpy(&nav, &navpoint_data[header.navpointsStart + (row * sizeof(Navpoint))], sizeof(Navpoint));

    ui_->navTable->insertRow(row);

    ui_->navTable->setItem(row, 0, new QTableWidgetItem(str));
    ui_->navTable->setItem(row, 1, new QTableWidgetItem(QString::number(*reinterpret_cast<int *>(&nav.unk0))));
    ui_->navTable->setItem(row, 2, new QTableWidgetItem(QString::number(nav.unk4, 'g', 4)));
    ui_->navTable->setItem(row, 3, new QTableWidgetItem(QString::number(nav.unk14, 'g', 4)));
    ui_->navTable->setItem(row, 4, new QTableWidgetItem(QString::number(nav.unk24, 'g', 4)));
    ui_->navTable->setItem(row, 5, new QTableWidgetItem(QString::number(nav.x, 'g', 4)));
    ui_->navTable->setItem(row, 6, new QTableWidgetItem(QString::number(nav.y, 'g', 4)));
    ui_->navTable->setItem(row, 7, new QTableWidgetItem(QString::number(nav.z, 'g', 4)));
}
