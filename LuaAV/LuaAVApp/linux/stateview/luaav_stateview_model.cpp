#include <QtGui>

#include "luaav_stateview_model.h"
#include "luaav_stateview_item.h"

StateViewModel :: StateViewModel(QObject *parent)
:	QAbstractItemModel(parent)
{
	QVector<QVariant> rootData;
	rootData << "Name" << "Value";
	mRoot = new StateViewItem(rootData);
}

StateViewModel :: ~StateViewModel() {
	delete mRoot;
}

StateViewItem * StateViewModel :: getItem(const QModelIndex &index) const {
	if(!index.isValid()) {
		return mRoot;
	}
	else {
		return static_cast<StateViewItem *>(index.internalPointer());
	}
}

int StateViewModel :: columnCount(const QModelIndex &parent) const {
	if(parent.isValid()) {
		return static_cast<StateViewItem *>(parent.internalPointer())->ncols();
	}
	else {
		return mRoot->ncols();
	}
}

QVariant StateViewModel :: data(const QModelIndex &index, int role) const {
	// if the index is not valid, return empty data
	if(!index.isValid()) {
		return QVariant();
	}

	/*
	switch(role) {
		case Qt::DisplayRole:		printf("modeldata DisplayRole\n"); break;
		case Qt::DecorationRole:	printf("modeldata DecorationRole\n"); break;
		case Qt::EditRole:			printf("modeldata EditRole\n"); break;
		case Qt::ToolTipRole:		printf("modeldata ToolTipRole\n"); break;
		case Qt::StatusTipRole:		printf("modeldata StatusTipRole\n"); break;
		case Qt::WhatsThisRole:		printf("modeldata WhatsThisRole\n"); break;
		case Qt::SizeHintRole:		printf("modeldata SizeHintRole\n"); break;
	}

	if(role == Qt::DecorationRole) {
		return QVariant(QColor(255, 0, 0));
	}
	*/

	// only handle display for right now
	if(role != Qt::DisplayRole) {
		return QVariant();
	}
	
	// get the data
	StateViewItem *item = static_cast<StateViewItem *>(index.internalPointer());
	return item->data(index.column());
}

Qt::ItemFlags StateViewModel :: flags(const QModelIndex &index) const {
	if (!index.isValid()) {
		return 0;
	}

	// everything is enabled and selectable
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; //| Qt::ItemIsEditable;
}

QVariant StateViewModel :: headerData(int section, 
								Qt::Orientation orientation,
								int role) const
{
	// use the root item to handle header data
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		return mRoot->data(section);
	}

	return QVariant();
}

QModelIndex StateViewModel :: index(int row, int column, 
									const QModelIndex &parent) const
{
//	printf("StateViewModel :: index\n");
	if(!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	StateViewItem *parentItem = getItem(parent);
	StateViewItem *childItem = parentItem->child(row);
	if(childItem) {
		return createIndex(row, column, childItem);
	}
	else {
		return QModelIndex();
	}
}

bool StateViewModel :: setData(const QModelIndex &index, const QVariant &value, int role) {
	if(role != Qt::EditRole) {
		return false;
	}

	StateViewItem *item = getItem(index);
	bool result = item->setData(index.column(), value);
	if(result) {
		emit dataChanged(index, index);
	}

	return result;
}

bool StateViewModel :: insertColumns(int position, int columns,
									const QModelIndex &parent)
{
	beginInsertColumns(parent, position, position+columns-1);
	bool success = mRoot->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool StateViewModel :: removeColumns(int position, int columns,
									const QModelIndex &parent)
{
	beginRemoveColumns(parent, position, position+columns-1);
	bool success = mRoot->removeColumns(position, columns);
	endRemoveColumns();

	if(mRoot->ncols() == 0) {
		removeRows(0, rowCount());
	}

	return success;
}

bool StateViewModel :: insertRows(int position, int rows,
									const QModelIndex &parent)
{
	StateViewItem *parentItem = getItem(parent);

	beginInsertRows(parent, position, position+rows-1);
	bool success = parentItem->insertChildren(position, rows, mRoot->ncols());
	endInsertRows();

	return success;
}

bool StateViewModel :: removeRows(int position, int rows,
									const QModelIndex &parent)
{
	StateViewItem *parentItem = getItem(parent);
	
	beginRemoveRows(parent, position, position+rows-1);
	bool success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

QModelIndex StateViewModel :: parent(const QModelIndex &index) const {
//	printf("StateViewModel :: parent\n");
	
	if(!index.isValid()) {
 		return QModelIndex();
	}

	StateViewItem *childItem = static_cast<StateViewItem *>(index.internalPointer());
	StateViewItem *parentItem = childItem->parent();

	if(parentItem == mRoot) {
		return QModelIndex();
	}

	return createIndex(parentItem->row(), 0, parentItem);
}

int StateViewModel :: rowCount(const QModelIndex &parent) const {
	if(parent.column() > 0) {
		return 0;
	}

	StateViewItem *item = getItem(parent);
	return item->nchildren();
}

QModelIndex StateViewModel :: namedRow(const QString &name) const {
	for(int i=0; i < mRoot->nchildren(); i++) {
		StateViewItem * c = mRoot->child(i);
		if(c->data(0).toString() == name) {
			return index(i, 0);
		}
	}
	return QModelIndex();
}


