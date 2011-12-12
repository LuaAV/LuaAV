#include <QStringList>
#include "luaav_stateview_item.h"
#include <stdio.h>


StateViewItem :: StateViewItem(const QVector<QVariant> &data, StateViewItem *parent)
:	mData(data),
	mParent(parent)
{}

StateViewItem :: ~StateViewItem() {
	qDeleteAll(mChildren);
}

bool StateViewItem :: insertChildren(int position, int count, int columns) {
	if(position < 0 || position > mChildren.size()) {
		return false;
	}

	for(int row = 0; row < count; row++) {
		QVector<QVariant> data(columns);
		StateViewItem *item = new StateViewItem(data, this);
		mChildren.insert(position, item);
	}

	return true;
}

bool StateViewItem :: removeChildren(int position, int count) {
	if(position < 0 || position+count > mChildren.size()) {
		return false;
	}

	for(int row = 0; row < count; row++) {
		delete mChildren.takeAt(position);
	}

	return true;
}

bool StateViewItem :: insertColumns(int position, int columns) {
	if(position < 0 || position > mData.size()) {
		return false;
	}

	for(int c = 0; c < columns; c++) {
		mData.insert(position, QVariant());
	}

	foreach(StateViewItem *child, mChildren)
			child->insertColumns(position, columns);

	return true;
}

bool StateViewItem :: removeColumns(int position, int columns) {
	if(position < 0 || position+columns > mData.size()) {
		return false;
	}

	for(int c = 0; c < columns; c++) {
		mData.remove(position);
	}

	foreach(StateViewItem *child, mChildren)
			child->removeColumns(position, columns);

	return true;
}

QVariant StateViewItem :: data (int column) const {
	return mData.value(column);
}

bool StateViewItem :: setData(int column, const QVariant &value) {
	if(column < 0 || column >= mData.size()) {
		return false;
	}

	mData[column] = value;
	return true;
}

StateViewItem * StateViewItem :: parent() {
	return mParent;
}

int StateViewItem :: row() const {
	if(mParent) {
		return mParent->mChildren.indexOf(const_cast<StateViewItem *>(this));
	}
	else {
		return 0;
	}
}

StateViewItem * StateViewItem :: child(int row){
	return mChildren.value(row);
}

int StateViewItem :: nchildren() const {
	return mChildren.count();
}

int StateViewItem :: ncols() const {
	return mData.count();
}

