/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_
#define ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_

#include <QString>
#include <iostream>
#include <memory>
#include <vector>

class QTextEdit;

class TreeEntry {
public:
    TreeEntry(QString name);
    TreeEntry(QString name, std::uint16_t index, std::uint32_t size);

    virtual ~TreeEntry();
    void removeAllChildren();
    void addChild(TreeEntry *entry);

    virtual void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr);

    std::uint16_t count() const;
    std::uint16_t getIndex() const;
    QString getName() const;
    QString getPath() const;
    QString getFilename() const;
    QString getExtension() const;
    qint64 getSize() const;
    TreeEntry *getParent() const;
    const std::uint8_t *getData() const;
    TreeEntry *getChild(std::uint16_t index) const;

    virtual bool is_archive() const = 0;

protected:
    TreeEntry *mParent {nullptr};
    std::vector<std::unique_ptr<TreeEntry> > mChildren;
    QString mName {""};
    QString mFilename {""};
    QString mExtension {""};
    bool mIsArchive {false};
    std::uint16_t mIndex {0};
    qint64 mSize {0};
    std::vector<std::uint8_t> mData;
};

#endif    // ACE3X_TREE_ENTRY_TREE_ENTRY_HPP_
