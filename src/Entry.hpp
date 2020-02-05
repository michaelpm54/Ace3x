/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <iostream>
#include <memory>
#include <vector>

#include <QString>

class QTextEdit;

class Entry {
public:
        Entry(QString name);
        Entry(QString name, std::uint16_t index, std::uint32_t size);

        virtual ~Entry();
        void removeAllChildren();
        void addChild(Entry *entry);

        virtual void read(std::vector<std::uint8_t> data, QTextEdit *log = nullptr);

        std::uint16_t      count() const;
        std::uint16_t      getIndex() const;
        QString            getName() const;
        QString            getPath() const;
        QString            getFilename() const;
        QString            getExtension() const;
        qint64             getSize() const;
        bool               isArchive() const;
        Entry              *getParent() const;
        const std::uint8_t *getData() const;
        Entry              *getChild(std::uint16_t index) const;

protected:
        Entry *mParent{ nullptr };
        std::vector<std::unique_ptr<Entry> > mChildren;
        QString mName{ "" };
        QString mFilename{ "" };
        QString mExtension{ "" };
        bool mIsArchive{ false };
        std::uint16_t mIndex{ 0 };
        qint64 mSize{ 0 };
        std::vector<std::uint8_t> mData;
};



#endif // ENTRY_HPP
