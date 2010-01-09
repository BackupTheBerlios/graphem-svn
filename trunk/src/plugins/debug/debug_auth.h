/*
    Graphem
    Copyright (C) 2009-2009 Christian Pulvermacher

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

#ifndef DEBUG_AUTH_H
#define DEBUG_AUTH_H

#include "auth.h"

#include <QtPlugin>

class InputWidget;
class NewGesture;
class QPainter;

/*
	This class acts as a proxy for another Auth plugin
	but can print input data to stdout.

	TODO: currently FCC is hardcoded
*/

class Debug : public Auth {
	Q_OBJECT
	Q_INTERFACES(Auth)
public:
	Debug();
	virtual ~Debug();

	virtual bool hashLoaded();
	virtual NewGesture* newGesture(); //create dialog for new gesture generation
	virtual void setInput(InputWidget *i);
public slots:
	virtual void check(); //checks data in InputWidget
	virtual void draw(QPainter* painter); // callback for drawing on InputWidget
	virtual void reset(); //reload gesture
signals:
	virtual void checkResult(bool correct);
protected:
	Auth* auth;
};
#endif
