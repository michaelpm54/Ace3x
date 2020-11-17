/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
#define ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_

#include <QWidget>
#include <memory>

#include "formats/p3d.hpp"
#include "widgets/format-viewers/viewer.hpp"

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
    struct P3DInternal {
        std::uint32_t verticesStart;
    };

private:
    void addNavpoint(const QString &str, const P3DHeader &header, const std::uint8_t *const navpoint_data);
    void writeVerticesToObj(const QString &fileName, const P3DHeader &header, const std::uint8_t *const vertex_data);

private slots:
    void onWriteObjClicked();

private:
    std::unique_ptr<Ui::P3DViewer> ui_;
    const VfsEntry *item_ {nullptr};
    P3DHeader p3d_header_;
};

#endif    // ACE3X_WIDGETS_FORMAT_VIEWERS_P3D_VIEWER_HPP_
