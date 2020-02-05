/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef FILEVIEWMODEL_HPP
#define FILEVIEWMODEL_HPP

#include <QAbstractItemModel>
#include <memory>

class Entry;

class FileViewModel : public QAbstractItemModel {
public:
        FileViewModel();
	~FileViewModel();

        void  addRoots(QList<Entry *> list);
        void  clear();
        Entry *itemFromIndex(const QModelIndex &index) const;

protected:
	QModelIndex index(int row, int col, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &index) const;
	QVariant    data(const QModelIndex &index, int role) const;
	bool        hasChildren(const QModelIndex &index) const;
	QVariant    headerData(int section, Qt::Orientation orientation, int role) const;
	int         rowCount(const QModelIndex &parent) const;
	int         columnCount(const QModelIndex&) const;
	bool        canFetchMore(const QModelIndex &index) const;
	void        fetchMore(const QModelIndex &index);

private:
	void populate(Entry *parent, const Entry &entry);
	void addChildren(Entry *parent, Entry *entry);

	std::unique_ptr<Entry> mInvisibleRoot;
};

#endif // FILEVIEWMODEL_HPP
