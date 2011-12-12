#ifndef LUAAV_STATEVIEW_MODEL_H
#define LUAAV_STATEVIEW_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class StateViewItem;

class StateViewModel : public QAbstractItemModel {
Q_OBJECT

public:
	StateViewModel(QObject *parent = 0);
	~StateViewModel();

	// return data stored under a given role
	QVariant data(const QModelIndex &index, int role) const;

	// flags for a given index
	Qt::ItemFlags flags(const QModelIndex &index) const;

	// data for the given role and selection in the header with 
	// the specified orientation
	QVariant headerData(int section, Qt::Orientation orientation,
					int role = Qt::DisplayRole) const;

	// returns the index of the item in the model for the given 
	// row, column and parent
	QModelIndex index(int row, int column,
					const QModelIndex &parent = QModelIndex()) const;

	// sets the role data for the item at index to value
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);


	bool insertColumns(int position, int columns,
					const QModelIndex &parent = QModelIndex());
	bool removeColumns(int position, int columns,
					const QModelIndex &parent = QModelIndex());
	bool insertRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());
	bool removeRows(int position, int rows,
					const QModelIndex &parent = QModelIndex());

	
	QModelIndex namedRow(const QString &name) const;


	// the parent of the model item with the given index
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

protected:
	StateViewItem * getItem(const QModelIndex &index) const;
	
	
	StateViewItem *mRoot;
};

#endif	// LUAAV_STATEVIEW_MODEL
