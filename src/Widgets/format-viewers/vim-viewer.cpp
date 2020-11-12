/* SPDX-License-Identifier: GPLv3-or-later */

#include "widgets/format-viewers/vim-viewer.hpp"

#include <QByteArray>
#include <QDebug>

#include "formats/vif-mesh.hpp"
#include "tree-entries/tree-entry.hpp"

float ieee_float(uint32_t f)
{
    static_assert(sizeof(float) == sizeof f, "`float` has a weird size.");
    float ret;
    memcpy(&ret, &f, sizeof(float));
    return ret;
}

QTableWidgetItem *uintItem(uint32_t n, int base)
{
    return new QTableWidgetItem(QString::number(n, base));
}

QTableWidgetItem *floatItem(uint32_t n)
{
    return new QTableWidgetItem(QString::number(ieee_float(n), 'G', 4));
}

QTableWidgetItem *floatItem(float n)
{
    return new QTableWidgetItem(QString::number(n, 'G', 4));
}

VIMViewer::VIMViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Ace3x - VIM Viewer");
    resize(800, 600);

    connect(ui.sub0List, &QTableWidget::itemSelectionChanged, this, &VIMViewer::sub0Changed);

    ui.sub0List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui.sub1List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui.sub2List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui.vifBoneList->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui.sub4List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
    ui.sub5List->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
}

void VIMViewer::activate(const TreeEntry *item)
{
    m_item = item;

    ui.texList->clear();

    ui.sub0List->clear();
    ui.sub1List->clear();
    ui.sub2List->clear();
    ui.vifBoneList->clear();
    ui.sub4List->clear();
    ui.sub5List->clear();

    ui.sub0Count->clear();
    ui.sub1Count->clear();
    ui.sub2Count->clear();
    ui.vifBoneCount->clear();
    ui.sub4Count->clear();
    ui.sub5Count->clear();

    show();

    m_vim.read(reinterpret_cast<const char *>(item->getData()));

    ui.texCount->setText(QString::number(m_vim.numTextures_0x1c_));
    for (int i = 0; i < m_vim.numTextures_0x1c_; i++)
        ui.texList->addItem(m_vim.textureNames_0x20_[i]);

    ui.sub0Count->setText(QString("Sub0 # %1").arg(m_vim.sub0_count_));
    ui.sub0List->setRowCount(m_vim.sub0_count_);
    ui.sub0List->setColumnCount(1);
    ui.sub0List->setHorizontalHeaderLabels({"VIM offset"});
    for (int i = 0; i < m_vim.sub0_count_; i++) {
        auto offset = m_vim.sub0_data_ + sizeof(VifMeshSub0) * i;
        ui.sub0List->setItem(i, 0, uintItem(offset, 16));
    }

    ui.vifBoneCount->setText(QString("vifBone # %1").arg(m_vim.vifBone_count_));
    ui.vifBoneList->setRowCount(m_vim.vifBone_count_);
    ui.vifBoneList->setColumnCount(28 + 2);
    ui.vifBoneList->setHorizontalHeaderLabels({"VIM offset", "Name offset", "Name", "x", "y", "z"});
    for (int row = 0; row < m_vim.vifBone_count_; row++) {
        auto offset = m_vim.vifBone_data_ + sizeof(VifBone) * row;
        auto *ptr = &m_item->getData()[offset];

        VifBone vifBone;
        memcpy(&vifBone, ptr, sizeof(VifBone));

        ui.vifBoneList->setItem(row, 0, uintItem(offset, 16));    // extra
        ui.vifBoneList->setItem(row, 1, uintItem(vifBone.boneNameOff, 16));
        ui.vifBoneList->setItem(row, 3, floatItem(vifBone.x));
        ui.vifBoneList->setItem(row, 4, floatItem(vifBone.y));
        ui.vifBoneList->setItem(row, 5, floatItem(vifBone.z));
        ui.vifBoneList->setItem(row, 6, floatItem(vifBone.a));
        ui.vifBoneList->setItem(row, 7, floatItem(vifBone.b));
        ui.vifBoneList->setItem(row, 8, floatItem(vifBone.c));

        QString name;
        ptr = m_item->getData() + vifBone.boneNameOff;
        while (*ptr != 0x0)
            name += *ptr++;

        ui.vifBoneList->setItem(row, 2, new QTableWidgetItem(name));    // extra

        for (int column = 7; column < 28; column++) {
            uint32_t *off = reinterpret_cast<uint32_t *>(&vifBone) + column;
            ui.vifBoneList->setItem(row, column + 2, floatItem(*off));
        }
    }

    ui.sub4Count->setText(QString("Sub4 # %1").arg(m_vim.sub4_count_));
    ui.sub4List->setRowCount(m_vim.sub4_count_);
    ui.sub4List->setColumnCount(15);
    ui.sub4List->setHorizontalHeaderLabels({"VIM offset"});
    for (int row = 0; row < m_vim.sub4_count_; row++) {
        auto offset = m_vim.sub4_data_ + (sizeof(VifMeshSub4) * row);

        VifMeshSub4 sub4;
        memcpy(&sub4, &m_item->getData()[offset], sizeof(VifMeshSub4));

        ui.sub4List->setItem(row, 0, uintItem(offset, 16));

        for (int column = 0; column < 14; column++) {
            uint32_t *off = reinterpret_cast<uint32_t *>(&sub4) + column;
            if (column == 0) {
                auto value = *off;
                auto str = QString::number(*off, 16);
                ui.sub4List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
            else {
                auto value = ieee_float(*off);
                auto str = QString::number(ieee_float(*off), 'G', 4);
                ui.sub4List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
        }
    }

    ui.sub5Count->setText(QString("Sub5 # %1").arg(m_vim.sub5_count_));
    ui.sub5List->setRowCount(m_vim.sub5_count_);
    ui.sub5List->setColumnCount(9);
    ui.sub5List->setHorizontalHeaderLabels({"VIM offset"});
    for (int row = 0; row < m_vim.sub5_count_; row++) {
        auto offset = m_vim.sub5_data_ + (sizeof(VifMeshSub5) * row);

        ui.sub5List->setItem(row, 0, new QTableWidgetItem(QString::number(offset, 16)));

        VifMeshSub5 sub5;
        memcpy(&sub5, &m_item->getData()[offset], sizeof(VifMeshSub5));

        for (int column = 0; column < 8; column++) {
            uint32_t *off = reinterpret_cast<uint32_t *>(&sub5) + column;
            //            if (column == 0)
            {
                auto value = *off;
                auto str = QString("0x%1").arg(value);
                ui.sub5List->setItem(row, column + 1, new QTableWidgetItem(str));
            }
            //            else
            {
                //                auto value = ieee_float(*off);
                //                auto str = QString("%1").arg(value);
                //                ui.sub4List->setItem(row, column, new QTableWidgetItem(str));
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
    memcpy(&sub0, &m_item->getData()[m_vim.sub0_data_ + (sizeof(VifMeshSub0) * ui.sub0List->currentRow())], sizeof(VifMeshSub0));

    ui.sub1Count->setText(QString("Sub1 # %1").arg(sub0.sub1_count));
    ui.sub1List->setRowCount(sub0.sub1_count);
    ui.sub1List->setColumnCount(14);
    ui.sub1List->setHorizontalHeaderLabels({"VIM offset", "?", "?", "?", "?", "Mesh #", "Offset0", "Offset1", "?", "Offset2", "Offset3", "Offset4", "Offset5", "Offset6"});
    for (int row = 0; row < sub0.sub1_count; row++) {
        auto offset = sub0.sub1_data + (sizeof(VifMeshSub1) * row);
        VifMeshSub1 sub1;
        memcpy(&sub1, &m_item->getData()[offset], sizeof(VifMeshSub1));

        int col = 0;
        ui.sub1List->setItem(row, col++, uintItem(offset, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.field_0x0, 10));
        ui.sub1List->setItem(row, col++, uintItem(sub1.field_0x2, 10));
        ui.sub1List->setItem(row, col++, uintItem(sub1.field_0x4, 10));
        ui.sub1List->setItem(row, col++, uintItem(sub1.field_0x6, 10));
        ui.sub1List->setItem(row, col++, uintItem(sub1.meshIdx, 10));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off0, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off1, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.field_0x14, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off2, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off3, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off4, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off5, 16));
        ui.sub1List->setItem(row, col++, uintItem(sub1.off6, 16));
    }

    ui.sub2Count->setText(QString("Sub2 # %1").arg(sub0.sub2_count));
    ui.sub2List->setRowCount(sub0.sub2_count);
    ui.sub2List->setColumnCount(4);
    ui.sub2List->setHorizontalHeaderLabels({"VIM offset", "Offset1"});
    for (int row = 0; row < sub0.sub2_count; row++) {
        auto offset = sub0.sub2_data + (sizeof(VifMeshSub2) * row);

        VifMeshSub2 sub2;
        memcpy(&sub2, &m_item->getData()[offset], sizeof(VifMeshSub2));

        ui.sub2List->setItem(row, 0, uintItem(offset, 16));
        ui.sub2List->setItem(row, 1, uintItem(sub2.off0, 16));
        ui.sub2List->setItem(row, 2, uintItem(sub2.field_0x4, 16));
        ui.sub2List->setItem(row, 3, uintItem(sub2.field_0x6, 16));
    }
}
