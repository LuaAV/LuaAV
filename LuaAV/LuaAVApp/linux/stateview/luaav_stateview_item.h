#ifndef LUAAV_STATEVIEW_ITEM_H
#define LUAAV_STATEVIEW_ITEM_H

#include <QList>
#include <QVector>
#include <QVariant>

class StateViewItem {
public:
	StateViewItem(const QVector<QVariant> &data, StateViewItem *parent = 0);
	~StateViewItem();

	// add/remove children
	bool insertChildren(int position, int count, int columns);
	bool removeChildren(int position, int count);

	// set/get the item columns
	bool insertColumns(int position, int columns);
	bool removeColumns(int position, int columns);

	// set/get the item data
	QVariant data(int column) const;
	bool setData(int column, const QVariant &value);

	// get the item's parent
	StateViewItem * parent();

	// get an item's child if it exists
	StateViewItem * child(int row);

	int row() const;
	int nchildren() const;
	int ncols() const;
	

protected:

	QList<StateViewItem *> mChildren;
	QVector<QVariant> mData;
	StateViewItem *mParent;
};

#endif	// LUAAV_STATEVIEW_ITEM
