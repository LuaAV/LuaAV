#include "luaav_stateview_qt.h"
#include "luaav_stateview_model.h"
#include "luaav_linux.h"
#include "luaav_app_stateview.h"

#include <QPixmap>
#include <QIcon>
#include <QTextEdit>
#include <cstring>

StateViewQt :: StateViewQt(QWidget *parent)
:	QTreeView(parent),
	mModel(0)
{
	mModel = new StateViewModel();
	setModel(mModel);
	setWindowIcon(QIcon(QPixmap(LUAAV_ICON_PATH)));
	setWindowTitle(QObject::tr("State View"));
	setGeometry(10, 100, 350, 500);
	show();
}

StateViewQt :: ~StateViewQt() {
	delete mModel;
}

namespace luaav {

class StateViewImpl{
public:
	StateViewImpl(StateView *generic)
	:	mGeneric(generic),
		mQtStateView(0)
	{}

	StateView * generic() {return mGeneric;}

	~StateViewImpl() {}

	StateView *mGeneric;
	StateViewQt *mQtStateView;
};

#define mQtStateView (mImpl->mQtStateView)

void StateView :: implInit() {
	mImpl = new Implementation(this);
}

void StateView :: implDestruct() {
	delete mImpl;
	mImpl = 0;
}

void StateView :: implCreate(int x, int y, int w, int h) {
	if(! mQtStateView) {
		mQtStateView = new StateViewQt();
	}
}

void StateView :: implDestroy() {
	if(mQtStateView) {
		delete mQtStateView;
		mQtStateView = 0;
	}
}

void setIndexWidget(StateViewQt *sv, const QModelIndex &index, const char *text) {
	// need to eventually add syntax highlighting
	QTextEdit *te = new QTextEdit();
	te->setTabStopWidth(35);
	te->setFrameShadow(QFrame::Plain);
	te->setFrameStyle(QFrame::NoFrame);
	te->setReadOnly(true);
	te->setLineWidth(0);
	te->textCursor().insertText(QString(text));

	sv->setIndexWidget(index, te);
}

bool replaceIndexWidgetText(StateViewQt *sv, const QModelIndex &index, const char *text) {
	QTextEdit *te = (QTextEdit *)sv->indexWidget(index);
	if(te) {
		QTextCursor tc = te->textCursor();
		tc.select(QTextCursor::Document);
		tc.insertText(QString(text));
		return true;
	}
	else {
		return false;
	}
}

void StateView :: implAddItem(const char *name, const char *type, const char *code) {
	if(mQtStateView) {
		// check if it's already in the model
		StateViewModel *model = mQtStateView->model();
		QModelIndex rowIdx = model->namedRow(QString(name));

		// if so, update it
		if(rowIdx.row() >= 0) {
			if(! replaceIndexWidgetText(mQtStateView, model->index(0, 1, rowIdx), code)) {
				model->setData(model->index(rowIdx.row(), 1), QVariant("function"), Qt::EditRole);

				if(model->insertRows(0, 1, rowIdx)) {
					// set the data for the sub row (<blank>, code)
					setIndexWidget(mQtStateView, model->index(0, 1, rowIdx), code);
				}
			}
		}
		// if not, create it
		else {
			// find last row in model and append one
			int nrows = model->rowCount();
			if(model->insertRows(nrows, 1)) {
				QModelIndex mainIdx = model->index(nrows, 0);

				// set the data for the main row (name, "function")
				model->setData(mainIdx, QVariant(name), Qt::EditRole);
				model->setData(model->index(nrows, 1), QVariant("function"), Qt::EditRole);

				if(model->insertRows(0, 1, mainIdx)) {
					// set the data for the sub row (<blank>, code)
					setIndexWidget(mQtStateView, model->index(0, 1, mainIdx), code);
				}
			}
		}
	}
}
/*
//	void implAddTable(const char *name, ???);

void StateView :: implAddString(const char *name, const char *v) {
	if(mQtStateView) {
		StateViewModel *model = mQtStateView->model();
		QModelIndex rowIdx = model->namedRow(QString(name));

		// if so, update it
		if(rowIdx.row() >= 0) {
			if(model->hasIndex(0, 0, rowIdx)) {
				model->removeRows(0, 1, rowIdx);
			}

			model->setData(model->index(rowIdx.row(), 1), QVariant(v), Qt::EditRole);
		}
		else {
			// find last row in model and append one
			int nrows = model->rowCount();
			if(model->insertRows(nrows, 1)) {
				QModelIndex mainIdx = model->index(nrows, 0);

				// set the data for the main row (name, v)
				model->setData(mainIdx, QVariant(name), Qt::EditRole);
				model->setData(model->index(nrows, 1), QVariant(v), Qt::EditRole);
			}
		}
	}
}

void StateView :: implAddNumber(const char *name, double v) {
	if(mQtStateView) {
		StateViewModel *model = mQtStateView->model();
		QModelIndex rowIdx = model->namedRow(QString(name));

		// if so, update it
		if(rowIdx.row() >= 0) {
			if(model->hasIndex(0, 0, rowIdx)) {
				model->removeRows(0, 1, rowIdx);
			}

			model->setData(model->index(rowIdx.row(), 1), QVariant(v), Qt::EditRole);
		}
		else {
			// find last row in model and append one
			int nrows = model->rowCount();
			if(model->insertRows(nrows, 1)) {
				QModelIndex mainIdx = model->index(nrows, 0);

				// set the data for the main row (name, v)
				model->setData(mainIdx, QVariant(name), Qt::EditRole);
				model->setData(model->index(nrows, 1), QVariant(v), Qt::EditRole);
			}
		}
	}
}
*/

}	// luaav::
