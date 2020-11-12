/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/p3d-viewer.hpp"

#include <QDir>
#include <array>
#include <fstream>

#include "tree-entries/tree-entry.hpp"

void P3DViewer::writeVerticesToObj(const std::uint8_t *const data, int start, int size)
{
}

void P3DViewer::writeVerticesToObj(const QString &fileName, const P3DHeader &header, const std::uint8_t *const data)
{
    std::vector<P3DSubHeader> subHeaders(header.numSubHeaders);
    memcpy(subHeaders.data(), data + header.filenamesEnd, sizeof(P3DSubHeader) * header.numSubHeaders);

    QString fnQt {QString("%1/%2.obj").arg(QDir::currentPath()).arg(fileName)};
    const std::string fnStd {fnQt.toStdString()};

    std::ofstream file(fnStd, std::ios::trunc);

    if (!file.good()) {
        std::clog << "[Error] Failed to open file '" << fnStd << "' for writing P3D vertices" << std::endl;
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
        const float *ptr = reinterpret_cast<const float *>(&data[subHeaders[i].offsetC]);

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

    std::clog << "[Info] Wrote P3D vertices to file '" << fnStd << "'" << std::endl;
}

P3DViewer::P3DViewer(QWidget *parent)
    : QWidget(parent)
{
    ui_.setupUi(this);

    resize(800, 600);

    setWindowTitle("Ace3x - P3D Viewer");

    ui_.navTable->setColumnCount(8);
    ui_.navTable->setHorizontalHeaderLabels({"Name", "?", "?", "?", "?", "x", "y", "z"});
    ui_.navTable->setSortingEnabled(true);

    connect(ui_.writeToObjButton, &QPushButton::clicked, this, &P3DViewer::onWriteObjClicked);
}

void P3DViewer::onWriteObjClicked()
{
    writeVerticesToObj(QString::fromStdString(item_->getFilename()), p3d_header_, item_->getData());
}

void P3DViewer::activate(const TreeEntry *item)
{
    ui_.objList->clear();
    ui_.imgList->clear();
    ui_.lyrList->clear();
    ui_.navTable->clearContents();

    show();

    item_ = item;

    auto ptr = item->getData();

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
            ui_.imgList->addItem(str);
        }
        else if (str.startsWith('$')) {
            // $player, $npc, $hostile
            if (str.startsWith("$p") || str.startsWith("$n") || str.startsWith("$h")) {
                addNavpoint(str, header, ptr);
            }
            else {
                ui_.lyrList->addItem(str);
            }
        }
        else {
            ui_.objList->addItem(str);
        }
    }

    ui_.objCount->setNum(ui_.objList->count());
    ui_.imgCount->setNum(ui_.imgList->count());
    ui_.lyrCount->setNum(ui_.lyrList->count());
    ui_.navCount->setNum(ui_.navTable->rowCount());

    ui_.navTable->resizeColumnsToContents();
    ui_.navTable->sortItems(0, Qt::SortOrder::AscendingOrder);
}

bool P3DViewer::shouldBeEnabled(const TreeEntry *) const
{
    return (true);
}

void P3DViewer::addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *const data)
{
    const int row = ui_.navTable->rowCount();

    Navpoint nav;
    memcpy(&nav, &data[header.navpointsStart + (row * sizeof(Navpoint))], sizeof(Navpoint));

    ui_.navTable->insertRow(row);

    ui_.navTable->setItem(row, 0, new QTableWidgetItem(str));
    ui_.navTable->setItem(row, 1, new QTableWidgetItem(QString::number(*reinterpret_cast<int *>(&nav.unk0))));
    ui_.navTable->setItem(row, 2, new QTableWidgetItem(QString::number(nav.unk4, 'g', 4)));
    ui_.navTable->setItem(row, 3, new QTableWidgetItem(QString::number(nav.unk14, 'g', 4)));
    ui_.navTable->setItem(row, 4, new QTableWidgetItem(QString::number(nav.unk24, 'g', 4)));
    ui_.navTable->setItem(row, 5, new QTableWidgetItem(QString::number(nav.x, 'g', 4)));
    ui_.navTable->setItem(row, 6, new QTableWidgetItem(QString::number(nav.y, 'g', 4)));
    ui_.navTable->setItem(row, 7, new QTableWidgetItem(QString::number(nav.z, 'g', 4)));
}
