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

#include "node.h"

#include <QLineF>
#include <QList>
#include <QString>
#include <QWidget>


class Auth;
class QTimer;

struct Arrow {
	bool pen_up;
	int start_node_id;
	int direction;
	int weight;
};

class InputWidget : public QWidget {
	Q_OBJECT
	friend class NewPattern;
public:
	InputWidget(QWidget *parent = 0, bool record = false);
	Auth* auth() { return _auth; } // TODO needed?
	void enableTouchpadMode(bool on);
	bool hashLoaded();
	void setDefaultMessage(QString m) { default_msg = m; }
signals:
	void dataReady();
public slots:
	void checkFinished();
	void deleteLastStroke();
	void exit();
	void printData();
	void quit();
	void reset();
	void showMessage(QString m = QString(), int msecs = 0);
protected:
	void mouseMoveEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void paintEvent(QPaintEvent *ev);
	void resizeEvent(QResizeEvent *ev);
private:
	QList<Node> path;
	QList<Arrow> arrows; // used when recording
	Auth *_auth;
	bool pen_down, mouse_down;
	QTimer *timer, *msg_timer;
	QString msg, default_msg;
	bool touchpad_mode, show_input;
	bool record_pattern;
	bool cursor_centered;
};
#endif
