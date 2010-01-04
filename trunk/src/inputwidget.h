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

#include "auth.h"
#include "node.h"

#include <QList>
#include <QString>
#include <QWidget>


class QPainter;
class QTimer;

class InputWidget : public QWidget {
	Q_OBJECT
public:
	InputWidget(QWidget *parent = 0, bool record = false);
	void enableTouchpadMode(bool on);
	void setDefaultMessage(QString m) { default_msg = m; }
	void setGrab(bool on) { do_grab = on; }

	QList<Node> path; //needed for Auth module
signals:
	void dataReady();
	void redraw(QPainter*);
public slots:
	void checkFinished();
	void fade();
	void focus();
	void printData();
	void reset();
	void showMessage(QString m = QString(), int msecs = 0);
protected:
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void paintEvent(QPaintEvent *ev);
	void resizeEvent(QResizeEvent *ev);
	void showEvent(QShowEvent *ev);
private:
	bool pen_down, mouse_down;
	QTimer *timer, *msg_timer;
	QString msg, default_msg;
	bool touchpad_mode, show_input;
	bool record_pattern;
	bool cursor_centered;
	bool do_grab;
	double fade_to; //final opacity after fade
};
#endif
