/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_

#include <QWidget>

#include "formats/p3d.hpp"
#include "ui_P3DViewer.h"
#include "widgets/format-viewers/viewer.hpp"

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
    Ui::P3DViewer ui_;
    const TreeEntry *item_ {nullptr};
    P3DHeader p3d_header_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
