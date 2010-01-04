/*
    Graphem
    Copyright (C) 2009 Christian Pulvermacher

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef FCC_NEWPATTERN_H
#define FCC_NEWPATTERN_H

#include "newpattern.h"

#include <QList>
#include <QPoint>
#include <QWidget>


class Auth;
class InputWidget;
class QPainter;
class QStatusBar;

struct Arrow {
	int start_node;
	int direction;
	int weight;
	bool pen_up;
};

class FCCNewPattern : public NewPattern {
	Q_OBJECT
public:
	FCCNewPattern(QWidget *parent, Auth *auth);
	void save();
private slots:
	void deleteLastStroke();
	void draw(QPainter *painter);
	void generate();
	void resetInput();
	void updateDisplay();
private:
	InputWidget *input;
	QStatusBar *status;
	bool touchpad_mode;

	QList<Arrow> arrows;
};
#endif
