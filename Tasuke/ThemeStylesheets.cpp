//@author A0100189M

#include "ThemeStylesheets.h"	

QStringList ThemeStylesheets::TASKWINDOW_STYLES = QStringList() 
	<< "QLabel,QPushButton { color:#666; } \n" // Theme 1: Default
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:#434343; } \n"
	"QLabel#border { border-radius:11px; background-color: rgb(74, 74, 74); }\n"
	"QLabel#bg { background:#FFF; border-radius:9px; } QPushButton#minButton { background-image:url(:/Images/images/theme1/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme1/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:#ABABAB;border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme1/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme1/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n"

	<< "QLabel,QPushButton { color:rgb(84,117,17); } \n" // Theme 2: Green
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(84,117,17); } \n"
	"QLabel#border { border-radius:11px; background-color: rgb(84,117,17); }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme2/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme2/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme2/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgb(195,223,140);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme2/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme2/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n" 

	<< "QLabel,QPushButton { color:white; } \n" // Theme 3: Space
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(228,235,255); } \n"
	"QLabel#border { border-radius:11px; background-color:black; }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme3/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme3/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme3/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgba(4,4,5,232);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme3/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme3/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n" 

	<< "QLabel,QPushButton { color:rgb(60,60,60); } \n" // Theme 4: Pink
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(215,137,156); } \n"
	"QLabel#border { border-radius:11px; background-color:rgb(236,169,177); }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme4/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme4/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme4/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgb(255,190,200);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme4/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme4/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n" 

	<< "QLabel,QPushButton { color:rgb(100,70,40); } \n" // Theme 5: Pika
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(120,89,49); } \n"
	"QLabel#border { border-radius:11px; background-color:rgb(192,150,100); }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme5/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme5/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme5/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgba(220,187,135,201);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme5/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme5/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n" 

	<< "QLabel,QPushButton { color:rgb(72,122,164); } \n" // Theme 6: Bluesky
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(56,103,180); } \n"
	"QLabel#border { border-radius:11px; background-color:rgb(132,174,215); }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme6/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme6/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme6/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgba(169,200,229,113);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme6/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme6/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n" 

	<< "QLabel,QPushButton { color:rgb(255,0,0); font:75 18pt \"Comic Sans MS\"; } \n" // Theme 7: doge
	"QWidget { background-color:transparent; } \n"
	"QPushButton {background-color:transparent;border:none;}\n"
	"QLabel#taskScope { color:rgb(0,255,255); font:75 25pt \"Comic Sans MS\";} \n"
	"QLabel#border { border-radius:11px; background-color:rgb(255,0,0); }\n"
	"QLabel#bg { background-image: url(:/Images/images/theme7/bg.png); border-radius:9px; } \n"
	"QPushButton#minButton { background-image:url(:/Images/images/theme7/minButton.png);}\n"
	"QPushButton#closeButton { background-image:url(:/Images/images/theme7/closeButton.png);}\n"
	"QScrollBar:vertical { border:none;background:transparent;width:7px;}\n"
	"QScrollBar::handle:vertical {background:rgb(255,255,0);border:none;width:7px;}\n"
	"QScrollBar::up-arrow:vertical,QScrollBar::down-arrow:vertical {border:none;background:none;width:0;height:0;}\n"
	"QToolTip {border:none; font:9pt Consolas; border-radius:2px; color:#FFF; background-color:gray; padding:2px;}\n"
	"QPushButton#minButton:hover,QPushButton#minButton:pressed {background-image:url(:/Images/images/theme7/minButtonHover.png);}\n"
	"QPushButton#closeButton:hover,QPushButton#closeButton:pressed {background-image:url(:/Images/images/theme7/closeButtonHover.png);}\n"
	"QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical {width:0;height:0;}\n";

QStringList ThemeStylesheets::TASKENTRY_NORMAL_STYLES = QStringList() 
	<< "QLabel#background{border-radius: 12px; background-color: rgb(203, 202, 202);}" // theme 1: default
	<< "QLabel#background{border-radius: 12px; background-color: rgb(195,223,140);}" // theme 2: green
	<< "QLabel#background{border-radius: 12px; background-color: rgba(17,21,36,200);}" // theme 3: space
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,222,235,200);}" // theme 4: pink
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,249,202,113);}" // theme 5: pika
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,255,255,113);}"  // theme 6: blue
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,255,0,155);}"; // theme 7: doge


QStringList ThemeStylesheets::TASKENTRY_SELECT_STYLES = QStringList() 
	<< "QLabel#background{border-radius: 12px; background-color: rgb(176, 175, 175);}" // theme 1: default
	<< "QLabel#background{border-radius: 12px; background-color: rgb(169,198,111);}" // theme 2: green
	<< "QLabel#background{border-radius: 12px; background-color: rgba(17,25,61,200);}" // theme 3: space
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,190,204,200);}" // theme 4: pink
	<< "QLabel#background{border-radius: 12px; background-color: rgba(220,187,135,201);}" // theme 5: pika
	<< "QLabel#background{border-radius: 12px; background-color: rgba(169,200,229,130);}"  // theme 6: blue
	<< "QLabel#background{border-radius: 12px; background-color: rgba(255,0,222,155);}"; // theme 7: doge

QStringList ThemeStylesheets::INPUTWINDOW_STYLES = QStringList() 
	<< "QLabel#bg{border-radius: 8px; background-color: white;}" // theme 1: default
	"QLabel#border{border-radius: 10px; background-color: rgb(74, 74, 74);}"
	"QTextEdit{background-color: transparent;}"
	"background:transparent;" 

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme2/bg.png);}" // theme 2: green
	"QLabel#border{border-radius: 10px; background-color: rgb(84,117,17);}"
	"QTextEdit{background-color: transparent; color: rgb(40,40,40);}"
	"background:transparent;" 

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme3/bg.png);}" // theme 3: space
	"QLabel#border{border-radius: 10px; background-color: black;}"
	"QTextEdit{background-color: transparent; color: white;}"
	"background:transparent;"

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme4/bg.png);}" // theme 4: pink
	"QLabel#border{border-radius: 10px; background-color: rgb(236,169,177);}"
	"QTextEdit{background-color: transparent; color: rgb(40,40,40);}"
	"background:transparent;"

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme5/bg.png);}" // theme 5: pika
	"QLabel#border{border-radius: 10px; background-color: rgb(192,150,100);}"
	"QTextEdit{background-color: transparent; color: rgb(120,89,49);}"
	"background:transparent;"

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme6/bg.png);}" // theme 6: blue
	"QLabel#border{border-radius: 10px; background-color: rgb(132,174,215);}"
	"QTextEdit{background-color: transparent;color: rgb(40,40,40);}"
	"background:transparent;"

	<< "QLabel#bg{border-radius: 8px; background-image:url(:/Images/images/theme7/inputbg.png);}" // theme 7: doge
	"QLabel#border{border-radius: 10px; background-color: rgb(255,0,222);}"
	"QTextEdit{background-color: transparent; color: rgb(255,0,0);	font: 75 25pt \"Comic Sans MS\";}"
	"background:transparent;";

QStringList ThemeStylesheets::SUBHEADING_STYLES = QStringList() 
	<< "QLabel{\n" // theme 1: default
	"color: rgb(99, 99, 99);\n"
	"}\n"
	"background: transparent;"

	<< "QLabel{\n" // theme 2: green
	"color: rgb(84,117,17);\n"
	"}\n"
	"background: transparent;"

	<< "QLabel{\n" // theme 3: space
	"color: rgb(228,235,255);\n"
	"}\n"
	"background: transparent;"

	<< "QLabel{\n" // theme 4: pink
	"color: rgb(215,137,156);\n"
	"}\n"
	"background: transparent;"

	<<"QLabel{\n" // theme 5: pika
	"color: rgb(120,89,49);\n"
	"}\n"
	"background: transparent;" 

	<< "QLabel{\n" // theme 6: blue
	"color: rgb(77,124,169);\n"
	"}\n"
	"background: transparent;"

	<< "QLabel{\n" // theme 7: doge
	"color: rgb(0,255,255);\n"
	"}\n"
	"background: transparent;"
	"font: 75 16pt \"Comic Sans MS\";";