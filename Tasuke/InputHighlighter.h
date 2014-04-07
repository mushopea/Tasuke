#pragma once
#include <QSyntaxHighlighter>
#include <QRegularExpression>

class InputHighlighter : public QSyntaxHighlighter {
     Q_OBJECT

 public:
     InputHighlighter(QTextDocument *parent = 0);
	 void setRegex();
	 void setFormats(QColor commandC, QColor keywordC, QColor connectorC);
	 void setRules();
	 void setupColorsFormatsRules(QColor commandC, QColor keywordC, QColor connectorC);

 protected:
     void highlightBlock(const QString &text);

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
};