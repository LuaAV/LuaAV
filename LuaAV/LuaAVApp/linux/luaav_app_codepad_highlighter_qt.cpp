#include <QtGui>
#include "luaav_app_codepad_highlighter_qt.h"
#include "luaav_app_codepad_qt.h"

#include <string>


 LuaHighlighterQt :: LuaHighlighterQt(CodePadQt *codepad, QTextDocument *parent)
:	mCodePad(codepad),
	QSyntaxHighlighter(parent)
{}

LuaHighlighterQt :: ~LuaHighlighterQt()
{}

void LuaHighlighterQt :: highlightBlock(const QString &text) {
	std::string line = text.toStdString();

//	printf("%d %d\n", currentBlock().blockNumber(), document()->blockCount());
	if(currentBlock().blockNumber()+1 != document()->blockCount()) {
		line.append("\n");
	}

	mCodePad->highlight(currentBlock().firstLineNumber(), line.data());
}
