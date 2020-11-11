/* SPDX-License-Identifier: GPLv3-or-later */

#include "P3DViewer.hpp"

#include <QDir>
#include <array>
#include <fstream>

#include "tree-entry/tree-entry.hpp"

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
    ui.setupUi(this);

    resize(800, 600);

    setWindowTitle("Ace3x - P3D Viewer");

    ui.navTable->setColumnCount(8);
    ui.navTable->setHorizontalHeaderLabels({"Name", "?", "?", "?", "?", "x", "y", "z"});
    ui.navTable->setSortingEnabled(true);

    connect(ui.writeToObjButton, &QPushButton::clicked, this, &P3DViewer::onWriteObjClicked);
}

void P3DViewer::onWriteObjClicked()
{
    writeVerticesToObj(m_item->getFilename(), m_header, m_item->getData());
}

void P3DViewer::activate(const TreeEntry *item)
{
    ui.objList->clear();
    ui.imgList->clear();
    ui.lyrList->clear();
    ui.navTable->clearContents();

    show();

    m_item = item;

    auto ptr = item->getData();

    P3DHeader header;
    memcpy(&header, ptr, sizeof(P3DHeader));
    header.size += 0x8;

    m_header = header;

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
            ui.imgList->addItem(str);
        }
        else if (str.startsWith('$')) {
            // $player, $npc, $hostile
            if (str.startsWith("$p") || str.startsWith("$n") || str.startsWith("$h")) {
                addNavpoint(str, header, ptr);
            }
            else {
                ui.lyrList->addItem(str);
            }
        }
        else {
            ui.objList->addItem(str);
        }
    }

    ui.objCount->setNum(ui.objList->count());
    ui.imgCount->setNum(ui.imgList->count());
    ui.lyrCount->setNum(ui.lyrList->count());
    ui.navCount->setNum(ui.navTable->rowCount());

    ui.navTable->resizeColumnsToContents();
    ui.navTable->sortItems(0, Qt::SortOrder::AscendingOrder);
}

bool P3DViewer::shouldBeEnabled(const TreeEntry *) const
{
    return (true);
}

void P3DViewer::addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *const data)
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
