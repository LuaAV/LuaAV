#include "luaav_app_codepad.h"
#include "luaav_app_codepad_qt.h"
#include "luaav_app_codepad_window_qt.h"
#include "luaav.h"
#include "lua_utility.h"
#include "luaav_app.h"
#include "luaav_linux.h"

#include <QAction>
#include <QTextDocumentFragment>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>

namespace luaav {
	
class CodePadImpl{
public:
	typedef CodePad		Generic;
	
	CodePadImpl(Generic *generic)
	:	mGeneric(generic),
		mCodePad(0)
	{}
	
	~CodePadImpl() {}
	
	Generic * generic() {return mGeneric;}
	
public:
	Generic *mGeneric;
	CodePadQt *mCodePad;
};

}


CodePadQt :: CodePadQt(CodePad *codePad, const char *name)
:	mCodePad(codePad),
	mLineArea(0),
	mHighlighter(0),
	mLines(0),
	mName(name)
{
//	setWindowTitle(tr("Code Pad"));
//	setWindowIcon(QIcon(QPixmap(LUAAV_ICON_PATH)));
	setTabStopWidth(25);
//	setGeometry(10, 10, 350, 400);

	mLineArea = new LineNumberArea(this);

	QAction *evalAction = new QAction(this);
	evalAction->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_Return));
	evalAction->setEnabled(true);
	connect(evalAction, SIGNAL(triggered()), this, SLOT(eval()));
	addAction(evalAction);

	connect(document(), SIGNAL(blockCountChanged(int)), 
			this, SLOT(updateLineNumberAreaWidth(int)));

	connect(document(), SIGNAL(blockCountChanged(int)), 
			this, SLOT(updateLineNumberArea(int)));

	connect(this, SIGNAL(cursorPositionChanged()), 
			this, SLOT(repaint()));

	connect(this, SIGNAL(cursorPositionChanged()), 
			this, SLOT(highlightCurrentLine()));

	connect(document(), SIGNAL(contentsChange(int,int,int)),
			this, SLOT(handleInsertDelete(int,int,int)));

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();

	mHighlighter = new LuaHighlighterQt(this, document());
}

CodePadQt :: ~CodePadQt() {
	if(mCodePad) {
		mCodePad->mImpl->mCodePad = 0;
	}
}

void CodePadQt :: setName(const char *name) {
	mName.assign(name);
	CodePadWindow *window = CodePadWindow::holder(this);
	if(window) {
		int idx = window->indexOf(this);
		window->setTabText(idx, QString(name));
	}
}

int CodePadQt :: lineNumberAreaWidth() {
	int digits = 3;
	int max = qMax(1, document()->blockCount());
	while (max > 100) {
		max /= 10;
		++digits;
	}

	int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;
	return space;
}



void CodePadQt :: updateLineNumberAreaWidth(int /* newBlockCount */) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void CodePadQt :: updateLineNumberArea(int) {
	mLineArea->repaint();
}

void CodePadQt :: resizeEvent(QResizeEvent *e) {
	QTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	mLineArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodePadQt :: scrollContentsBy (int dx, int dy) {
	QTextEdit::scrollContentsBy(dx, dy);
	mLineArea->repaint();
}


void CodePadQt :: highlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> selections = extraSelections();

	QTextEdit::ExtraSelection selection;
	QColor lineColor = QColor(Qt::yellow).lighter(160);

	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = textCursor();
	selection.cursor.clearSelection();

	if(selections.size() <= 0) {
		selections.append(selection);
	}
	else {
		selections.replace(0, selection);
	}

	setExtraSelections(selections);
}

void CodePadQt :: errorHighlight(int line) {
	QList<QTextEdit::ExtraSelection> selections = extraSelections();

	QTextEdit::ExtraSelection selection;
	QColor lineColor = QColor(Qt::red).lighter(100);

	selection.format.setBackground(lineColor);
	selection.format.setProperty(QTextFormat::FullWidthSelection, true);
	selection.cursor = textCursor();

	// move to position where the line of the error is
	QTextBlock block = document()->findBlockByLineNumber(line);
	selection.cursor.setPosition(block.position());

	selection.cursor.clearSelection();
	
	if(selections.size() <= 1) {
		selections.append(selection);
	}
	else {
		selections.replace(1, selection);
	}

	setExtraSelections(selections);
}

void CodePadQt :: clearErrorHighlight() {
	QList<QTextEdit::ExtraSelection> selections = extraSelections();
	if(selections.size() >= 1) {	
		selections.removeAt(1);
		setExtraSelections(selections);
	}
}


 void CodePadQt :: lineNumberAreaPaintEvent(QPaintEvent *event) {
	QPainter painter(mLineArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QWidget *vp = viewport();
	QTextCursor tc = cursorForPosition(QPoint(vp->x(), vp->y()));
	QTextBlock block = tc.block();

	QRect cr = cursorRect(tc);
	int top = cr.y();

	while(block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible()) {
			tc.setPosition(block.position()); 
			cr = cursorRect(tc);
			top = cr.y();

			QString number = QString::number(block.firstLineNumber() + 1);
			painter.setPen(Qt::black);
			painter.drawText(-3, top, mLineArea->width(), fontMetrics().height(),
						Qt::AlignRight, number);
		}

		block = block.next();
	}
}

void CodePadQt :: handleInsertDelete(int position, int removed, int added) {
	QTextDocument *doc = document();
	QTextBlock current = doc->findBlock(position);
	int nlines_changed = document()->blockCount() - mLines;

/*	printf("(%d >= %d && %d <= 1) || %d == 0\n", 
		current.blockNumber(),
		mLines-1,
		nlines_changed,
		nlines_changed);
*/	
	if(nlines_changed < 0) {
		// delete
//		printf("REMOVE\n");
		mCodePad->remove(current.blockNumber(), nlines_changed);
	}
	else if((current.blockNumber() >= (mLines-1) && nlines_changed <= 1) ||
		nlines_changed == 0)
	{
		// normal replace, add at the end
		//printf("NORMAL\n");
	}
	else {
	//	printf("INSERT LINES: %d at %d\n", nlines_changed, current.blockNumber());
		mCodePad->insert(current.blockNumber(), nlines_changed);
	}

	mLines = document()->blockCount();
}

void CodePadQt :: eval() {
	if(mCodePad) {
		QString code = textCursor().selection().toPlainText();
		mCodePad->eval(code.toStdString().data());
	}
}

void CodePadQt :: highlight(int line, const char *text) {
	if(mCodePad) {
		mCodePad->highlight(line, text);
	}
}

void CodePadQt :: formatText(const char *format, int start, int end) {
	std::map<std::string, QTextCharFormat>::iterator it = mFormats.find(std::string(format));
	if(it != mFormats.end()) {
		// apply formatting
		//mHighlighter->doHighlight(start, end, it->second);
		QTextCursor c(document());
		c.setPosition(start);
		c.setPosition(end, QTextCursor::KeepAnchor);
		c.setCharFormat(it->second);
	}
}

void CodePadQt :: createFormat(const char *name, float *color, 
								std::map<std::string, int> &properties)
{
	QTextCharFormat format;
	format.setForeground(QColor((int)(color[0]*255), (int)(color[1]*255), (int)(color[2]*255)));
	
	std::map<std::string, int>::iterator it = properties.begin();
	for(; it != properties.end(); ++it) {
		if(it->first == std::string("font_weight")) {
			format.setFontWeight(it->second);
		}
		else if(it->first == std::string("italic")) {
			format.setFontItalic(it->second ? true : false);
		}
		else if(it->first == std::string("underline")) {
			format.setFontUnderline(it->second ? true : false);
		}
	}

	mFormats.insert(std::pair<std::string, QTextCharFormat>(std::string(name), format));
}

void CodePadQt :: insertText(int pos, const char *text) {
	QTextCursor cursor = textCursor();
	cursor.setPosition(pos);
	cursor.insertText(QString(text));
}

namespace luaav {


#define mQtCodePad (mImpl->mCodePad)


void CodePad :: implInit() {
	mImpl = new Implementation(this);
}

void CodePad :: implDestruct() {
	delete mImpl;
	mImpl = 0;
}


void CodePad :: implCreate(const char *name, int x, int y, int w, int h) {
	if(!mQtCodePad) {
		mQtCodePad =  new CodePadQt(this, name);
		CodePadWindow *window = CodePadWindow::activeWindow();
		window->add(mQtCodePad, mQtCodePad->getName());
		
//		mQtCodePad->setGeometry(10, 100, 350, 500);
//		mQtCodePad->show();
	}
}

void CodePad :: implDestroy() {
	if(mQtCodePad) {
		CodePadWindow *window = CodePadWindow::holder(mQtCodePad);
		if(window) {
			CodePadWindow::remove(window, mQtCodePad);
		}

		if(mQtCodePad) {
			delete mQtCodePad;
			mQtCodePad = 0;
		}
	}
}

void CodePad :: implSetName(const char *name) {
	if(mQtCodePad) {
		mQtCodePad->setName(name);
	}
}

const char * CodePad :: implGetName() {
	if(mQtCodePad) {
		return mQtCodePad->getName();
	}
	else {
		return "<invalid>";
	}
}

const char * CodePad :: implGetCurrentName() {
	CodePadWindow *window = CodePadWindow::existingActiveWindow();
	if(window) {
		CodePadQt *cp = (CodePadQt *)window->currentWidget();
		if(cp) {
			return cp->getName();
		}
	}
	
	return NULL;
}

void CodePad :: implSetText(const char *text) {
	if(mQtCodePad) {
		mQtCodePad->document()->setPlainText(QString(text));
	}
}

std::string CodePad :: implGetText() {
	if(mQtCodePad) {
		QString text = mQtCodePad->document()->toPlainText();
		return text.toStdString();
	}
	else {
		return std::string();
	}
}

int CodePad :: implCursorPos() {
	if(mQtCodePad) {
		return mQtCodePad->textCursor().position();
	}
	else {
		return 0;
	}
}

void CodePad :: implPaste(int pos, const char *text) {
	if(mQtCodePad) {
		mQtCodePad->insertText(pos, text);
	}
}

void CodePad :: implCreateFormat(const char *name, float *color, 
								std::map<std::string, int> &properties)
{
	if(mQtCodePad) {
		mQtCodePad->createFormat(name, color, properties);
	}
}

void CodePad :: implFormat(const char *name, int start, int end) {
	if(mQtCodePad) {
		mQtCodePad->formatText(name, start, end);
	}
}

void CodePad :: implGetSelection(int &line1, int &pos1, int &line2, int &pos2) {
	if(mQtCodePad) {
		QTextDocument *doc = mQtCodePad->document();	
		int p1 = mQtCodePad->textCursor().selectionStart();
		int p2 = mQtCodePad->textCursor().selectionEnd();

		QTextBlock block1 = doc->findBlock(p1);
		QTextBlock block2 = doc->findBlock(p2);

		line1 = block1.blockNumber();
		pos1 = p1 - block1.position();

		line2 = block2.blockNumber();
		pos2 = p2 - block2.position();
	}
}

void CodePad :: implErrorHighlight(int line) {
	if(mQtCodePad) {
		mQtCodePad->errorHighlight(line);
	}
}

void CodePad :: implClearErrorHighlight() {
	if(mQtCodePad) {
		mQtCodePad->clearErrorHighlight();
	}
}

}	// luaav::

