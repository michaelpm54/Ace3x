/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef P3DVIEWER_HPP
#define P3DVIEWER_HPP

#include <QWidget>

#include "Viewer.hpp"
#include "formats/P3D.hpp"
#include "ui_P3DViewer.h"

class TreeEntry;
struct P3DHeader;

class P3DViewer : public QWidget, public Viewer {
    Q_OBJECT
public:
    explicit P3DViewer(QWidget *parent = nullptr);

    void activate(const TreeEntry *item) override;
    bool shouldBeEnabled(const TreeEntry *item) const override;

private:
    struct P3DInternal {
        std::uint32_t verticesStart;
    };

private:
    void addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *const data);
    void writeVerticesToObj(const QString &fileName, const P3DHeader &header, const std::uint8_t *const data);
    void writeVerticesToObj(const std::uint8_t *const data, int start, int size);

private slots:
    void onWriteObjClicked();

private:
    Ui::P3DViewer ui;
    const TreeEntry *m_item {nullptr};
    P3DHeader m_header;
};

#endif    // P3DVIEWER_HPP
