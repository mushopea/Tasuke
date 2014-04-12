#include "Tasuke.h"
#include "InputHighlighter.h"
#include <QSettings>

//@author A0100189

InputHighlighter::InputHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent), commandsEnabled(true), spellcheckEnabled(true) {
	setRegex();
	setupColorsFormatsRules();
}


// ========================================
// GETTERS & SETTERS
// ========================================
bool InputHighlighter::getCommandsEnabled() const {
	return commandsEnabled;
}

void InputHighlighter::setCommandsEnabled(bool newEnabled) {
	commandsEnabled = newEnabled;
}

bool InputHighlighter::getSpellcheckEnabled() const {
	return spellcheckEnabled;
}

void InputHighlighter::setSpellcheckEnabled(bool newEnabled) {
	spellcheckEnabled = newEnabled;
}


// ========================================
// HANDLES HIGHLIGHTING
// ========================================

// For setting up of formats and rules
void InputHighlighter::setupColorsFormatsRules() {
	setFormats(QColor(14, 158, 235), QColor(133, 76, 180), QColor(135, 180, 0));
	setRules();
}

// Goes through entire text and highlights anything that matches the rules.
void InputHighlighter::highlightBlock(const QString &text) {
	// Highlights keywords
	if (commandsEnabled) {
		foreach (const HighlightingRule &rule, highlightingRules) {
			QRegularExpressionMatchIterator matchIter = rule.pattern.globalMatch(text);
			while (matchIter.hasNext()) {
				QRegularExpressionMatch match = matchIter.next();
				for (int i = 1; i <= match.lastCapturedIndex(); ++i) {
					setFormat(match.capturedStart(i), match.capturedLength(i), rule.format);
				}
			}
		}
	}

	// Does spellcheck
	if (spellcheckEnabled) {
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
}

// Define the regular expressions for different types of words
void InputHighlighter::setRegex(){	
	commandRegex = QRegularExpression("(^(((much)|(such)|(wow such)|(wow much)|(so)|(many)) )?"
									  "((\\b(add)|(create)|(a)|(do)|"
									  "(remove)|(delete)|(clear)|(rm)|"
									  "(edit)|(update)|(modify)|(change)|(e)|"
									  "(show)|(display)|(list)|(find)|(search)|(ls)|"
									  "(undo)|(redo)|(r)|(u)|(done)|(undone)|(d)|(nd))\\b)+)");

	keywordRegex = QRegularExpression("(^\\b((much|such|wow such|wow much|so|many) )?"
									  "((\\b(exit)|(quit)|(q)|(options)|(settings)|(help)|(tutorial)|(guide)|(instructions)|(about))\\b)+)");
	
	connectorRegex = QRegularExpression("^(?:(?:wow\\s+)?(?:[ms]uch|so|many)\\s+)?" // doge prefixes
										"(?:add|create|a|do|edit|update|modify|change|e)" // commands with connectors
										".+" // item being added or edited
										"(from|by|at|on|@)\\b(?:\\s+(?:\\S+\\s+)+(to|-)\\b)?"); // connectors, with optional "to" connector
   
	tagRegex = QRegularExpression("\\s(#)\\S");
}

// Set the formatting of the text displayed
void InputHighlighter::setFormats(QColor commandC, QColor keywordC, QColor connectorC){
	commandFormat.setForeground(commandC);
	keywordFormat.setForeground(keywordC);
	connectorFormat.setForeground(connectorC);
	spellCheckFormat.setUnderlineColor(QColor(Qt::red));
	spellCheckFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
}

// Adds to highlighting rules 
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

	HighlightingRule tagRule;
	tagRule.pattern = tagRegex;
	tagRule.format = connectorFormat;
	highlightingRules.append(tagRule);
}
