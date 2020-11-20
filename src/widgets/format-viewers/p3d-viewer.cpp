/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/p3d-viewer.hpp"

#include <spdlog/spdlog.h>

#include <QDir>
#include <array>
#include <fstream>

#include "ui_p3d-viewer.h"
#include "vfs/vfs-entry.hpp"

void P3DViewer::write_vertices(const QString &fileName, const P3DHeader &header, const unsigned char *const item_data)
{
    std::vector<P3DObjInfo> obj_info(header.num_sub1_0x14);
    std::memcpy(obj_info.data(), item_data + header.ptr_sub1_0x18, sizeof(P3DObjInfo) * header.num_sub1_0x14);

    const auto filename = fmt::format("{}.obj", fileName.toStdString());

    std::ofstream file(filename, std::ios::trunc);

    if (!file.good()) {
        spdlog::error("P3D: Failed to open file '{}' for writing vertices", filename);
        return;
    }

    std::vector<std::vector<std::array<float, 3>>> meshes(header.num_sub1_0x14);

    for (auto i = 0u; i < header.num_sub1_0x14; i++) {
        const float *vertex_list = reinterpret_cast<const float *>(&item_data[obj_info[i].ptr_vertices_0xC]);

        u32 offset = obj_info[i].ptr_vertices_0xC;

        /* Read the number of vertices then fetch them until the value is obviously
        * not a vertex count. For this I'll use < 500. */
        u32 num_vertices = 0;
        do {
            num_vertices = reinterpret_cast<const u32 *>(vertex_list)[3];

            float vertices[3];
            std::memcpy(vertices, vertex_list, 12);
            meshes[i].push_back({vertices[0], vertices[1], vertices[2]});
            vertex_list += 4;
            offset += 16;

            if (num_vertices >= 500)
                break;

            for (auto v = 1u; v < num_vertices; v++) {
                std::memcpy(vertices, vertex_list, 12);

                /* Invert each vertex depending on the 4th value.
                * This isn't the correct use of the 4th value (+1.0f or -1.0f)
                * but I can't figure out what is. */
                /*
                for (int j = 0; j < 3; j++) {
                    vertices[j] *= vertex_list[3];
                }
                */

                meshes[i].push_back({vertices[0], vertices[1], vertices[2]});
                vertex_list += 4;
                offset += 16;
            }
        } while (num_vertices < 500 && offset < 0x2000);

        spdlog::info("Mesh {} has {} sets of vertices", i, meshes[i].size());
    }

    for (const auto &mesh : meshes) {
        /* Each mesh is an object. */
        file << "o Object\n";

        /* 3 positions per vertex. */
        for (const auto &vertex : mesh) {
            file << "v " << vertex[0] << ' ' << vertex[1] << ' ' << vertex[2] << '\n';
        }

        /* Creates a monolithic face for each mesh. Face indices start at 1. */
        int index = 1;
        file << "f ";
        for (int i = 0; i < mesh.size(); i++) {
            file << index++ << ' ';
        }
        file << '\n';
    }

    /* Write each navpoint as an object with 1 vertex. */
    if (ui_->write_navpoints_cb->isChecked()) {
        for (const auto &navpoint : navpoints_) {
            file << "o Object\n";
            file << "v " << navpoint.x << ' ' << navpoint.y << ' ' << navpoint.z << '\n';
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
    connect(ui_->writeToObjButton, &QPushButton::clicked, this, &P3DViewer::onWriteObjClicked);
}

void P3DViewer::onWriteObjClicked()
{
    write_vertices(QString::fromStdString(item_->name), header_, item_->data);
}

void P3DViewer::activate(const VfsEntry *item)
{
    ui_->objList->clear();
    ui_->imgList->clear();

    show();

    item_ = item;

    auto ptr = item->data;

    std::memcpy(&header_, ptr, sizeof(P3DHeader));

    load_navpoints(item->data, header_.num_navpoints, header_.ptr_navpoints);
    load_layers(item->data, header_.num_layers, header_.ptr_layers);

    int num_navpoint_names_found = 0;

    std::uint32_t namesStart = sizeof(P3DHeader);
    std::uint32_t namesEnd = header_.ptr_sub1_0x18;
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
            if (str.startsWith("$player") || str.startsWith("$npc") || str.startsWith("$hostile")) {
                ui_->navpoint_table->setItem(num_navpoint_names_found++, 0, new QTableWidgetItem(str));
            }
        }
        else {
            ui_->objList->addItem(str);
        }
    }

    ui_->objCount->setNum(ui_->objList->count());
    ui_->imgCount->setNum(ui_->imgList->count());
}

bool P3DViewer::shouldBeEnabled(const VfsEntry *) const
{
    return (true);
}

void P3DViewer::load_navpoints(const unsigned char *data, u32 count, u32 offset)
{
    navpoints_.resize(count);
    std::memcpy(navpoints_.data(), data + offset, count * sizeof(P3DNavpoint));

    ui_->navpoint_count->setNum(static_cast<double>(count));

    ui_->navpoint_table->clear();
    ui_->navpoint_table->setRowCount(count);
    ui_->navpoint_table->setColumnCount(8);
    ui_->navpoint_table->setHorizontalHeaderLabels({"Name", "?", "?", "?", "?", "x", "y", "z"});
    ui_->navpoint_table->setSortingEnabled(true);
    ui_->navpoint_table->sortItems(0, Qt::SortOrder::AscendingOrder);

    for (auto i = 0u; i < count; i++) {
        const auto &navpoint = navpoints_[i];
        ui_->navpoint_table->setItem(i, 1, new QTableWidgetItem(QString::number(*reinterpret_cast<const u32 *>(&navpoint.unk_0x0))));
        ui_->navpoint_table->setItem(i, 2, new QTableWidgetItem(QString::number(navpoint.unk_0x4, 'g', 4)));
        ui_->navpoint_table->setItem(i, 3, new QTableWidgetItem(QString::number(navpoint.unk_0x14, 'g', 4)));
        ui_->navpoint_table->setItem(i, 4, new QTableWidgetItem(QString::number(navpoint.unk_0x24, 'g', 4)));
        ui_->navpoint_table->setItem(i, 5, new QTableWidgetItem(QString::number(navpoint.x, 'g', 4)));
        ui_->navpoint_table->setItem(i, 6, new QTableWidgetItem(QString::number(navpoint.y, 'g', 4)));
        ui_->navpoint_table->setItem(i, 7, new QTableWidgetItem(QString::number(navpoint.z, 'g', 4)));
    }
}

void P3DViewer::load_layers(const unsigned char *data, u32 count, u32 offset)
{
    std::vector<P3DLayer> layers(count);
    std::memcpy(layers.data(), data + offset, count * sizeof(P3DLayer));

    ui_->layer_count->setNum(static_cast<double>(count));

    ui_->layer_table->clear();
    ui_->layer_table->setRowCount(count);
    ui_->layer_table->setColumnCount(3);
    ui_->layer_table->setHorizontalHeaderLabels({"Name", "# objects", "Offset"});

    for (auto i = 0u; i < count; i++) {
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
        ui_->layer_table->setItem(i, 2, new QTableWidgetItem("0x" + QString::number(layers[i].ptr_0xC, 16)));
    }
}
