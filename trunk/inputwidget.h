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

#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include <QString>
#include <QTime>

#include "node.h"

class QTimer;

class InputWidget : public QWidget {
	Q_OBJECT
private:
	bool pen_down, mouse_down;
	QTimer *timer;
	QString title, msg;
	bool touchpad_mode, lock_screen, show_input;
public:
	InputWidget(QWidget *parent = 0);
	void enableLocking(bool b);
	void enableTouchpadMode(bool b);
	void reset();
	void show();
	void showInput(bool b);

	QList<Node> path;
protected:
	void mouseMoveEvent(QMouseEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void tabletEvent(QTabletEvent *ev);
	void paintEvent(QPaintEvent *ev);
signals:
	void finished();
public slots:
	void checkFinished();
	void printData();
	void showMessage(QString m = "Please enter your auth pattern.", int msecs = 0);
};
#endif
