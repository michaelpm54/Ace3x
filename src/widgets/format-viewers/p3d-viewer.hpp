/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "formats/p3d.hpp"
#include "widgets/format-viewers/viewer.hpp"

using u32 = std::uint32_t;

namespace Ui {
class P3DViewer;
}

struct P3DHeader;

class P3DViewer : public Viewer {
    Q_OBJECT
public:
    explicit P3DViewer(QWidget *parent = nullptr);

    void activate(const VfsEntry *item) override;
    bool shouldBeEnabled(const VfsEntry *item) const override;

private:
    void write_vertices(const QString &fileName, const P3DHeader &header, const unsigned char *const item_data);
    void load_navpoints(const unsigned char *data, u32 count, u32 offset);
    void load_layers(const unsigned char *data, u32 count, u32 offset);

private slots:
    void onWriteObjClicked();

private:
    std::unique_ptr<Ui::P3DViewer> ui_;
    const VfsEntry *item_ {nullptr};
    P3DHeader header_;
    std::vector<P3DNavpoint> navpoints_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
