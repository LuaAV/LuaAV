#ifndef LUAAV_STATEVIEW_QT_H
#define LUAAV_STATEVIEW_QT_H 1

#include <QTreeView>

class StateViewModel;

class StateViewQt : public QTreeView {
Q_OBJECT

public:
	StateViewQt(QWidget *parent = 0);
	~StateViewQt();

	StateViewModel * model() {return mModel;}

protected:
	StateViewModel *mModel;
};

#endif	// LUAAV_STATEVIEW_QT_H
