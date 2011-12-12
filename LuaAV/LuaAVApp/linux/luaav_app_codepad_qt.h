#ifndef LUAAV_APP_CODEPAD_QT_H
#define LUAAV_APP_CODEPAD_QT_H

#include <QTextEdit>
#include <QTextCharFormat>
#include <map>
#include <string>
#include "luaav_app_codepad.h"
#include "luaav_app_codepad_highlighter_qt.h"

using luaav::CodePad;

class LineNumberArea;


class CodePadQt : public QTextEdit{
	Q_OBJECT

public:
	CodePadQt(CodePad *codePad, const char *name);
	virtual ~CodePadQt();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

	void highlight(int line, const char *text);
	void formatText(const char *format, int start, int end);

	/*
		properties:
			font_weight = value,
			italic = 0/1,
			underline = 0/1,
	*/
	void createFormat(const char *name, float *color, 
						std::map<std::string, int> &properties);
	void errorHighlight(int line);
	void clearErrorHighlight();
	void setName(const char *name);
	const char * getName() {return mName.data();}
	void insertText(int pos, const char *text);

protected:
	void resizeEvent(QResizeEvent *event);
	void scrollContentsBy (int dx, int dy);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(int);
	void handleInsertDelete(int position, int removed, int added);

	void eval();	

protected:
	CodePad *mCodePad;
	LineNumberArea *mLineArea;
	LuaHighlighterQt *mHighlighter;
	std::map<std::string, QTextCharFormat> mFormats;
	int mLines;
	std::string mName;
};

class LineNumberArea : public QWidget {
public:
	LineNumberArea(CodePadQt *codePad)
	:	QWidget(codePad),
		mCodePad(codePad)
	{}

	~LineNumberArea() {}
	
	QSize sizeHint() const {
		return QSize(mCodePad->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event) {
		mCodePad->lineNumberAreaPaintEvent(event);
	}

	CodePadQt *mCodePad;
};

#endif	// LUAAV_APP_CODEPAD_QT_H
