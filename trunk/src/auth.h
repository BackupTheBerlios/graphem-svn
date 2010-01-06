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

#ifndef AUTH_H
#define AUTH_H

#include <QObject>

class InputWidget;
class NewGesture;
class QPainter;

class Auth : public QObject {
	Q_OBJECT
public:
	Auth(QObject *parent, InputWidget *input) : QObject(parent), input(input) { }
	virtual ~Auth() { }

	virtual bool hashLoaded() = 0;
	virtual NewGesture* newGesture() = 0; //create dialog for new gesture generation
public slots:
	virtual void check() = 0; //checks data in InputWidget
	virtual void draw(QPainter* /*painter*/) { } // callback for drawing on InputWidget
	virtual void reset() = 0; //reload gesture
signals:
	virtual void checkResult(bool correct);
protected:
	InputWidget *input;
};
#endif
