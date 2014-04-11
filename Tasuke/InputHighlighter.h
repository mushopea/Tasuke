#pragma once
#include <QSyntaxHighlighter>
#include <QRegularExpression>

//@author A0100189

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
	QRegularExpression commandRegex;
	QRegularExpression keywordRegex;
	QRegularExpression connectorRegex;
	QRegularExpression tagRegex;

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