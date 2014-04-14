#pragma once
#include <QSyntaxHighlighter>
#include <QRegularExpression>

//@author A0100189m

// This class is a subclass of QSyntaxHighlighter.
// It handles color highlighting of input window's text's commands keywords and spellcheck.
// To add new keywords, simply edit the regular expressions in setRegex().
// To add new colors and styles, add a new format and rule in setRules().

class InputHighlighter : public QSyntaxHighlighter {
	Q_OBJECT

public:
	InputHighlighter(QTextDocument *parent = 0);
	bool getCommandsEnabled() const;
	void setCommandsEnabled(bool newEnabled);
	bool getSpellcheckEnabled() const;
	void setSpellcheckEnabled(bool newEnabled);

public slots:	
	void setupColorsFormatsRules();

protected:
	void highlightBlock(const QString &text) override;

private:
	// regular expressions that define keywords
	QRegularExpression commandRegex;
	QRegularExpression keywordRegex;
	QRegularExpression connectorRegex;
	QRegularExpression tagRegex;

	// style of the keyword group
	QTextCharFormat commandFormat;
	QTextCharFormat keywordFormat;
	QTextCharFormat connectorFormat;
	QTextCharFormat spellCheckFormat;

	struct HighlightingRule {
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	bool commandsEnabled;
	bool spellcheckEnabled;

	void setRegex();
	void setFormats(QColor commandC, QColor keywordC, QColor connectorC);
	void setRules();
};