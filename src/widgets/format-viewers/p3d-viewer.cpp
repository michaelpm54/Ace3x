/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/p3d-viewer.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <array>
#include <fstream>

#include "ui_p3d-viewer.h"
#include "vfs/vfs-entry.hpp"

using u16 = std::uint16_t;
using u32 = std::uint32_t;
using f32 = float;

struct sub_structure_0 {
    /* 0000014A, 0000014B, 00000152, 00000154 */
    u32 unk_0x0;
    /* FFFF FFFF 0005 0005 */
    u16 unk_0x4;
    /* FFFF FFFF 0004 0004 */
    u16 unk_0x6;
    /* FFFFFFFF FFFFFFFF 00000003 00000002 */
    u32 unk_0x8;
    f32 unk_0xC;
    f32 unk_0x10;
    f32 unk_0x14;
    f32 unk_0x18;
    f32 unk_0x1C;
    f32 unk_0x20;
    f32 unk_0x24;
    f32 unk_0x28;
    f32 unk_0x2C;
    f32 unk_0x30;
    f32 unk_0x34;
    f32 unk_0x38;
    f32 unk_0x3C;
    f32 unk_0x40;
    f32 unk_0x44;
};

struct p3d_layer {
    u32 layer_name;
    u32 num_objects;
    u32 ofs_0xC;
};

void P3DViewer::writeVerticesToObj(const QString &fileName, const P3DHeader &header, const std::uint8_t *const vertex_data)
{
    std::vector<P3DSubHeader> subHeaders(header.numSubHeaders);
    memcpy(subHeaders.data(), vertex_data + header.filenamesEnd_xStart, sizeof(P3DSubHeader) * header.numSubHeaders);

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
    ui_->navTable->clearContents();

    show();

    item_ = item;

    auto ptr = item->data;

    P3DHeader header;
    memcpy(&header, ptr, sizeof(P3DHeader));
    header.size += 0x8;

    p3d_header_ = header;

    std::uint32_t namesStart = sizeof(P3DHeader);
    std::uint32_t namesEnd = header.filenamesEnd_xStart;
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
                //ui_->lyrList->addItem(str);
            }
        }
        else {
            ui_->objList->addItem(str);
        }
    }

    ui_->objCount->setNum(ui_->objList->count());
    ui_->imgCount->setNum(ui_->imgList->count());
    ui_->navCount->setNum(ui_->navTable->rowCount());

    ui_->navTable->resizeColumnsToContents();
    ui_->navTable->sortItems(0, Qt::SortOrder::AscendingOrder);

    load_layers(item->data, header.num_layers_2C, header.ptr_layers_30);
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

void P3DViewer::load_layers(const unsigned char *data, u32 count, u32 offset)
{
    std::vector<p3d_layer> layers(count);
    std::memcpy(layers.data(), data + offset, count * sizeof(p3d_layer));

    ui_->layer_count->setNum(static_cast<double>(count));

    ui_->layer_table->clear();
    ui_->layer_table->setRowCount(count);
    ui_->layer_table->setColumnCount(3);
    ui_->layer_table->setHorizontalHeaderLabels({"Name", "# objects", "Offset"});

    for (auto i = 0; i < count; i++) {
        char name[16];
        for (int j = 0; j < 16; j++) {
            name[j] = data[layers[i].layer_name + j];
            if (name[j] == '\0') {
                break;
            }
        }
        name[15] = '\0';

        ui_->layer_table->setItem(i, 0, new QTableWidgetItem(name));
        ui_->layer_table->setItem(i, 1, new QTableWidgetItem(QString::number(layers[i].num_objects)));
        ui_->layer_table->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(layers[i].ofs_0xC, 16)));
    }
}
