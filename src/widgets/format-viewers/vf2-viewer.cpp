/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/vf2-viewer.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>

#include "format-readers/peg.hpp"
#include "format-readers/vf2.hpp"
#include "formats/vf2.hpp"
#include "ui_vf2-viewer.h"
#include "vfs/vfs-entry.hpp"
#include "vfs/vfs.hpp"

namespace Ui {
class Vf2Viewer;
}

Vf2Viewer::Vf2Viewer(Vfs *vfs, QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::Vf2Viewer())
    , vfs_(vfs)
{
    ui_->setupUi(this);
    setWindowTitle("Ace3x - Font Viewer");
}

void Vf2Viewer::activate(const VfsEntry *item)
{
    show();

    Vf2Header header;
    std::memcpy(&header, item->data, sizeof(header));

    ui_->signature->setText(QString::fromLocal8Bit(QByteArray::fromRawData((const char *)(&header.signature), 4)));
    ui_->version->setText(QString::number(header.version));
    ui_->num_chars->setText(QString::number(header.num_chars));
    ui_->height->setText(QString::number(header.height));

    ui_->chars->clear();
    ui_->chars->setColumnCount(4);
    ui_->chars->setHorizontalHeaderLabels({"Min Width", "Max Width", "Pixel Offset", "Kerning Backspace"});
    ui_->chars->setRowCount(header.num_chars);
    ui_->chars->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui_->image_label->clear();

    struct FontChar {
        int min_width;
        int max_width;
        std::uint32_t pixel_offset;
        std::uint32_t kerning_back_pixels_width;
    };

    std::uint64_t chars_offset = sizeof(header);
    while (chars_offset + 4u < item->size) {
        std::uint32_t value {0};
        std::memcpy(&value, item->data + chars_offset, 4);
        if (value == 0xFFFF) {
            break;
        }
        chars_offset += 4;
    }
    chars_offset -= 12;

    std::uint64_t chars_size = sizeof(FontChar) * header.num_chars;

    if (chars_size + chars_offset >= item->size) {
        return;
    }

    std::vector<FontChar> chars(header.num_chars);
    std::memcpy(chars.data(), item->data + chars_offset, chars_size);

    for (auto i = 0u; i < header.num_chars; i++) {
        ui_->chars->setVerticalHeaderItem(i, new QTableWidgetItem(QChar(header.first_ascii + i)));
        ui_->chars->setItem(i, 0, new QTableWidgetItem(QString::number(chars[i].min_width)));
        ui_->chars->setItem(i, 1, new QTableWidgetItem(QString::number(chars[i].max_width)));
        ui_->chars->setItem(i, 2, new QTableWidgetItem(QString::number(chars[i].pixel_offset, 16)));
        ui_->chars->setItem(i, 3, new QTableWidgetItem(chars[i].kerning_back_pixels_width == 0xFFFF ? "None" : QString::number(chars[i].kerning_back_pixels_width)));
    }

    const auto summoner_path = std::filesystem::path(item->root->absolute_path).parent_path().generic_string() + '/' + "SUMMONER.VPP";
    emit request_load(QString::fromStdString(summoner_path));

    auto peg_name = QString::fromStdString(item->name).contains("pal") ? summoner_path + "/start0-keep-pal.peg" : summoner_path + "/start0-keep.peg";
    auto *peg = vfs_->get_entry(peg_name);
    if (!peg) {
        spdlog::error("VF2: Failed to find '{}'", peg_name);
        return;
    }

    auto images = ace3x::peg::get_images(peg->data);

    auto vbm_name = item->name.substr(0, item->name.size() - 4) + ".vbm";

    ace3x::peg::Image *vbm {nullptr};
    for (auto &image : images) {
        if (image.filename == vbm_name) {
            vbm = &image;
            break;
        }
    }

    if (!vbm) {
        spdlog::error("VF2: Failed to find '{}/{}'", peg_name, vbm_name);
        return;
    }

    const auto qt_image = QImage(reinterpret_cast<const unsigned char *>(vbm->pixels.data()), vbm->width, vbm->height, QImage::Format_ARGB32);
    ui_->image_label->setPixmap(QPixmap::fromImage(qt_image));
}

bool Vf2Viewer::shouldBeEnabled(const VfsEntry *) const
{
    return true;
}
