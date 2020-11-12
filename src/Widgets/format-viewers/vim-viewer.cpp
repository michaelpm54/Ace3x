/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/vim-viewer.hpp"

#include <QByteArray>
#include <QDebug>

#include "formats/vif-mesh.hpp"
#include "tree-entries/tree-entry.hpp"
#include "ui_vim-viewer.h"

float ieee_float(uint32_t f)
{
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    std::memcpy(&ret, &f, sizeof(float));
    return ret;
}

QTableWidgetItem *uintItem(std::uint32_t n, int base)
{
    return new QTableWidgetItem(QString::number(n, base));
}

QTableWidgetItem *floatItem(std::uint32_t n)
{
    return new QTableWidgetItem(QString::number(ieee_float(n), 'G', 4));
}

QTableWidgetItem *floatItem(float n)
{
    return new QTableWidgetItem(QString::number(n, 'G', 4));
}

VIMViewer::VIMViewer(QWidget *parent)
    : QWidget(parent)
    , ui_(new Ui::VIMViewer())
{
    ui_->setupUi(this);
    setWindowTitle("Ace3x - VIM Viewer");
    resize(800, 600);

    connect(ui_->sub0List, &QTableWidget::itemSelectionChanged, this, &VIMViewer::sub0Changed);

    ui_->sub0List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui_->sub1List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui_->sub2List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui_->vifBoneList->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui_->sub4List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui_->sub5List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
}

void VIMViewer::activate(const TreeEntry *item)
{
    item_ = item;

    ui_->texList->clear();

    ui_->sub0List->clear();
    ui_->sub1List->clear();
    ui_->sub2List->clear();
    ui_->vifBoneList->clear();
    ui_->sub4List->clear();
    ui_->sub5List->clear();

    ui_->sub0Count->clear();
    ui_->sub1Count->clear();
    ui_->sub2Count->clear();
    ui_->vifBoneCount->clear();
    ui_->sub4Count->clear();
    ui_->sub5Count->clear();

    show();

    vim_.read(reinterpret_cast<const char *>(item->getData()));

    ui_->texCount->setText(QString::number(vim_.numTextures_0x1c_));
    for (std::uint32_t i = 0u; i < vim_.numTextures_0x1c_; i++)
        ui_->texList->addItem(vim_.textureNames_0x20_[i]);

    ui_->sub0Count->setText(QString("Sub0 # %1").arg(vim_.sub0_count_));
    ui_->sub0List->setRowCount(vim_.sub0_count_);
    ui_->sub0List->setColumnCount(1);
    ui_->sub0List->setHorizontalHeaderLabels({"VIM offset"});

    for (std::uint32_t i = 0u; i < vim_.sub0_count_; i++) {
        std::uint32_t offset = vim_.sub0_data_ + sizeof(VifMeshSub0) * i;
        ui_->sub0List->setItem(i, 0, uintItem(offset, 16));
    }

    ui_->vifBoneCount->setText(QString("vifBone # %1").arg(vim_.vifBone_count_));
    ui_->vifBoneList->setRowCount(vim_.vifBone_count_);
    ui_->vifBoneList->setColumnCount(28 + 2);
    ui_->vifBoneList->setHorizontalHeaderLabels({"VIM offset", "Name offset", "Name", "x", "y", "z"});

    for (std::uint32_t row = 0u; row < vim_.vifBone_count_; row++) {
        std::uint32_t offset = vim_.vifBone_data_ + sizeof(VifBone) * row;
        auto *ptr = &item_->getData()[offset];

        VifBone vifBone;
        std::memcpy(&vifBone, ptr, sizeof(VifBone));

        ui_->vifBoneList->setItem(row, 0, uintItem(offset, 16));    // extra
        ui_->vifBoneList->setItem(row, 1, uintItem(vifBone.boneNameOff, 16));
        ui_->vifBoneList->setItem(row, 3, floatItem(vifBone.x));
        ui_->vifBoneList->setItem(row, 4, floatItem(vifBone.y));
        ui_->vifBoneList->setItem(row, 5, floatItem(vifBone.z));
        ui_->vifBoneList->setItem(row, 6, floatItem(vifBone.a));
        ui_->vifBoneList->setItem(row, 7, floatItem(vifBone.b));
        ui_->vifBoneList->setItem(row, 8, floatItem(vifBone.c));

        QString name;
        ptr = item_->getData() + vifBone.boneNameOff;
        while (*ptr != 0x0)
            name += *ptr++;

        ui_->vifBoneList->setItem(row, 2, new QTableWidgetItem(name));    // extra

        for (int column = 7; column < 28; column++) {
            uint32_t *off = reinterpret_cast<uint32_t *>(&vifBone) + column;
            ui_->vifBoneList->setItem(row, column + 2, floatItem(*off));
        }
    }

    ui_->sub4Count->setText(QString("Sub4 # %1").arg(vim_.sub4_count_));
    ui_->sub4List->setRowCount(vim_.sub4_count_);
    ui_->sub4List->setColumnCount(15);
    ui_->sub4List->setHorizontalHeaderLabels({"VIM offset"});

    for (std::uint32_t row = 0u; row < vim_.sub4_count_; row++) {
        std::uint32_t offset = vim_.sub4_data_ + (sizeof(VifMeshSub4) * row);

        VifMeshSub4 sub4;
        std::memcpy(&sub4, &item_->getData()[offset], sizeof(VifMeshSub4));

        ui_->sub4List->setItem(row, 0, uintItem(offset, 16));

        for (int column = 0; column < 14; column++) {
            uint32_t *off = reinterpret_cast<uint32_t *>(&sub4) + column;
            if (column == 0) {
                auto str = QString::number(*off, 16);
                ui_->sub4List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
            else {
                auto str = QString::number(ieee_float(*off), 'G', 4);
                ui_->sub4List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
        }
    }

    ui_->sub5Count->setText(QString("Sub5 # %1").arg(vim_.sub5_count_));
    ui_->sub5List->setRowCount(vim_.sub5_count_);
    ui_->sub5List->setColumnCount(9);
    ui_->sub5List->setHorizontalHeaderLabels({"VIM offset"});

    for (std::uint32_t row = 0u; row < vim_.sub5_count_; row++) {
        std::uint32_t offset = vim_.sub5_data_ + (sizeof(VifMeshSub5) * row);

        ui_->sub5List->setItem(row, 0, new QTableWidgetItem(QString::number(offset, 16)));

        VifMeshSub5 sub5;
        std::memcpy(&sub5, &item_->getData()[offset], sizeof(VifMeshSub5));

        for (int column = 0; column < 8; column++) {
            std::uint32_t *off = reinterpret_cast<uint32_t *>(&sub5) + column;
            //            if (column == 0)
            {
                auto value = *off;
                auto str = QString("0x%1").arg(value);
                ui_->sub5List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
            //            else
            {
                //                auto value = ieee_float(*off);
                //                auto str = QString("%1").arg(value);
                //                ui_->sub4List->setItem(row, column, new QTableWidgetItem(str));
            }
        }
    }
}

bool VIMViewer::shouldBeEnabled(const TreeEntry *) const
{
    return true;
}

void VIMViewer::sub0Changed()
{
    VifMeshSub0 sub0;
    std::memcpy(&sub0, &item_->getData()[vim_.sub0_data_ + (sizeof(VifMeshSub0) * ui_->sub0List->currentRow())], sizeof(VifMeshSub0));

    ui_->sub1Count->setText(QString("Sub1 # %1").arg(sub0.sub1_count));
    ui_->sub1List->setRowCount(sub0.sub1_count);
    ui_->sub1List->setColumnCount(14);
    ui_->sub1List->setHorizontalHeaderLabels({"VIM offset", "?", "?", "?", "?", "Mesh #", "Offset0", "Offset1", "?", "Offset2", "Offset3", "Offset4", "Offset5", "Offset6"});

    for (std::uint32_t row = 0u; row < sub0.sub1_count; row++) {
        std::uint32_t offset = sub0.sub1_data + (sizeof(VifMeshSub1) * row);

        VifMeshSub1 sub1;
        std::memcpy(&sub1, &item_->getData()[offset], sizeof(VifMeshSub1));

        int col = 0;
        ui_->sub1List->setItem(row, col++, uintItem(offset, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.field_0x0, 10));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.field_0x2, 10));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.field_0x4, 10));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.field_0x6, 10));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.meshIdx, 10));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off0, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off1, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.field_0x14, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off2, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off3, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off4, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off5, 16));
        ui_->sub1List->setItem(row, col++, uintItem(sub1.off6, 16));
    }

    ui_->sub2Count->setText(QString("Sub2 # %1").arg(sub0.sub2_count));
    ui_->sub2List->setRowCount(sub0.sub2_count);
    ui_->sub2List->setColumnCount(4);
    ui_->sub2List->setHorizontalHeaderLabels({"VIM offset", "Offset1"});

    for (std::uint32_t row = 0u; row < sub0.sub2_count; row++) {
        std::uint32_t offset = sub0.sub2_data + (sizeof(VifMeshSub2) * row);

        VifMeshSub2 sub2;
        std::memcpy(&sub2, &item_->getData()[offset], sizeof(VifMeshSub2));

        ui_->sub2List->setItem(row, 0, uintItem(offset, 16));
        ui_->sub2List->setItem(row, 1, uintItem(sub2.off0, 16));
        ui_->sub2List->setItem(row, 2, uintItem(sub2.field_0x4, 16));
        ui_->sub2List->setItem(row, 3, uintItem(sub2.field_0x6, 16));
    }
}
