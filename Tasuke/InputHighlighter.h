#pragma once
#include <QSyntaxHighlighter>


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
     QRegExp commandRegex;
     QRegExp keywordRegex;
	 QRegExp connectorRegex;

	 QTextCharFormat commandFormat;
     QTextCharFormat keywordFormat;
     QTextCharFormat connectorFormat;

	 struct HighlightingRule {
         QRegExp pattern;
         QTextCharFormat format;
     };

     QVector<HighlightingRule> highlightingRules;
};