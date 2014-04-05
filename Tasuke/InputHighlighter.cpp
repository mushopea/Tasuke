#include "Tasuke.h"
#include "InputHighlighter.h"

InputHighlighter::InputHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent){
	setRegex();
	setupColorsFormatsRules(QColor(51, 204, 255), Qt::darkCyan, QColor(164, 219, 0));
	spellCheckFormat.setUnderlineColor(QColor(Qt::red));
	spellCheckFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
}

void InputHighlighter::setRegex(){
	// Regex notes:
	// ^ means it will only be highlighted if the word is used at the start of the string
	// ? means 0 or more
	// + means at least one
	// \b means start/end boundary
	
	commandRegex = QRegExp("(^\\b(((much)|(such)|(wow such)|(wow much)|(so)|(many)) )?"
							"((\\b(add)|(create)|(a)|(do)|"
							"(remove)|(delete)|(clear)|(rm)|"
							"(edit)|(update)|(modify)|(change)|(e)|"
							"(show)|(display)|(list)|(find)|(search)|(ls)|"
							"(undo)|(redo)|(r)|(u)|(done)|(notdone)|(d)|(nd))\\b)+)");

	keywordRegex = QRegExp("(^\\b(((much)|(such)|(wow such)|(wow much)|(so)|(many)) )?"
							"((\\b(exit)|(quit)|(q)|(options)|(settings)|(help)|(tutorial)|(guide)|(instructions)|(about))\\b)+)");
	
	connectorRegex = QRegExp("((( to )|( from )|( by )|( at )|( on ))|( @)|-|#)");
}

void InputHighlighter::setFormats(QColor commandC, QColor keywordC, QColor connectorC){
	commandFormat.setForeground(commandC);
	keywordFormat.setForeground(keywordC);
	connectorFormat.setForeground(connectorC);
}

void InputHighlighter::setRules(){
	HighlightingRule commandRule;
	commandRule.pattern = commandRegex;
	commandRule.format = commandFormat;
	highlightingRules.append(commandRule);

	HighlightingRule keywordRule;
	keywordRule.pattern = keywordRegex;
	keywordRule.format = keywordFormat;
	highlightingRules.append(keywordRule);

	HighlightingRule connectorRule;
	connectorRule.pattern = connectorRegex;
	connectorRule.format = connectorFormat;
	highlightingRules.append(connectorRule);
}


void InputHighlighter::setupColorsFormatsRules(QColor commandC, QColor keywordC, QColor connectorC){
	setFormats(commandC, keywordC, connectorC);
	setRules();
}

void InputHighlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

	QStringList words = text.simplified().split(" ", QString::SkipEmptyParts);
	foreach(QString word, words) {
		if (word.size() > 1) {
			bool correct = Tasuke::instance().spellCheck(word);
			if (!correct) {
				int numOccurence = text.count(QRegExp("\\b"+word+"\\b"));
				int index = -1;
				// underline all occurences of misspelled word
				for (int i=0; i<numOccurence; i++) {
					index = text.indexOf(QRegExp("\\b"+word+"\\b"), index+1);
					if (index >= 0) {
						setFormat(index, word.length(), spellCheckFormat);
					}
				}
			}
		}
	}
}