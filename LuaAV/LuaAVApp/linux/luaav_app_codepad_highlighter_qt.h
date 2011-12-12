#ifndef LUAAV_APP_CODEPAD_HIGHLIGHTER_QT_H
#define LUAAV_APP_CODEPAD_HIGHLIGHTER_QT_H

 #include <QSyntaxHighlighter>

 #include <QHash>
 #include <QTextCharFormat>

class QTextDocument;
class CodePadQt;

class LuaHighlighterQt : public QSyntaxHighlighter{
	Q_OBJECT

public:
	LuaHighlighterQt(CodePadQt *codepad, QTextDocument *parent = 0);
	~LuaHighlighterQt();

	void doHighlight(int start, int end, QTextCharFormat format) {
		setFormat(start, end, format);
	}

protected:
	void highlightBlock(const QString &text);

private:
	struct HighlightingRule {
		QRegExp pattern;
		QTextCharFormat format;
	};

	CodePadQt *mCodePad;
};


#endif	// LUAAV_APP_CODEPAD_HIGHLIGHTER_QT_H
