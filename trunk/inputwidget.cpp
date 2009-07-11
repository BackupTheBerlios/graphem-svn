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

#include "inputwidget.h"

#include <cmath>
#include <iostream>

#include <QMouseEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>


InputWidget::InputWidget(QWidget* parent, bool record) :
	QWidget(parent),
	pen_down(false),
	mouse_down(false),
	timer(new QTimer(this)),
	msg_timer(new QTimer(this)),
	msg(""),
	default_msg("Please enter your auth pattern."),
	touchpad_mode(false),
	show_input(false),
	record_pattern(record)
{
	setMinimumSize(300,200);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	msg_timer->setSingleShot(true);
	connect(msg_timer, SIGNAL(timeout()),
		this, SLOT(showMessage()));

	timer->setInterval(70); //check every 70ms
	if(record_pattern) {
		connect(timer, SIGNAL(timeout()),
			this, SIGNAL(dataReady()));
		reset();
	} else {
		connect(timer, SIGNAL(timeout()),
			this, SLOT(checkFinished()));
	}
	showMessage();
	timer->start();
}


void InputWidget::checkFinished()
{
	if(path.isEmpty() or path.count() == 1) //we need at least 2 events
		return;
	showMessage(""); //don't show message after input started

	if(path.last().time.secsTo(QTime::currentTime()) >= 1) {
		showMessage("Processing...");
		repaint();
		emit dataReady();
	}
}


void InputWidget::enableTouchpadMode(bool on)
{
	touchpad_mode = on;
	if(!record_pattern)
		setMouseTracking(touchpad_mode);
}


void InputWidget::reset()
{
	path.clear();
	arrows.clear();
	if(!record_pattern)
		showMessage("Pattern not recognized, please try again.", 1500);
	update();
}


void InputWidget::showMessage(QString m, int msecs)
{
	msg_timer->stop(); //cancel timer if a timed message is being shown
	if(m.isNull())
		m = default_msg;

	if(msg != m)
		update(); //repaints after returning to event loop
	msg = m;
	if(msecs)
		msg_timer->start(msecs);
}


void InputWidget::mousePressEvent(QMouseEvent* ev)
{
	if(pen_down or touchpad_mode)
		return;

	mouse_down = true;

	path.append(Node(ev->pos()));
}


void InputWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if(pen_down or !mouse_down or touchpad_mode)
		return;

	mouse_down = false;

	path.append(Node(ev->pos()));
	path.append(Node::makeSeparator(ev->pos()));
	update();
}


void InputWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(pen_down) {
		ev->ignore();
		return;
	}

	if(mouse_down or touchpad_mode) {
		path.append(Node(ev->pos()));
		update();
	}
	//there are some move events with pressed buttons discarded here, as some mousePress events get lost
}


void InputWidget::paintEvent(QPaintEvent* /*ev*/)
{
	QPainter painter(this);
	painter.setPen(Qt::white);
	painter.fillRect(rect(), QBrush(Qt::black));

	painter.drawText(width()/2 - 100, height()/2, msg);
	if(touchpad_mode and !record_pattern)
		painter.drawText(11, 19, "[Touchpad Mode]");

	if(show_input and !path.isEmpty()) {
		QPainterPath painter_path;
		painter.setPen(Qt::red);

		for(int i=0; i < path.count(); i++) {
			if(path.at(i).separator)
				continue;
			if(i == 0 or path.at(i-1).separator)
				painter_path.moveTo(path.at(i).pos);
			else
				painter_path.lineTo(path.at(i).pos);
			painter.drawEllipse(path.at(i).pos.x()-1, path.at(i).pos.y()-1, 2, 2);
		}
		painter.setPen(Qt::blue);
		painter.drawPath(painter_path);
	}

	if(!record_pattern)
		return;
	for(int i=0; i < arrows.count(); i++) {
		if(arrows.at(i).pen_up)
			painter.setPen(Qt::red);
		else
			painter.setPen(Qt::white);

		QPointF start = path.at(arrows.at(i).start_node_id).pos;
		const double angle = arrows.at(i).direction / 4.0 * 3.14159;
		const QPointF end = start + arrows.at(i).weight*QPointF(cos(angle), -sin(angle));
		const QLineF l(start, end);
		if(l.length() == 0)
				continue;

		painter.drawLine(l);

		start = end + QPointF(-10*(l.dy()+l.dx())/l.length(), 10*(l.dx()-l.dy())/l.length());
		std::cout << "s.x " << start.x() << "; s.y " << start.y() << "\n";
		painter.drawLine(start, end);

		start = end + QPointF(10*(l.dy()-l.dx())/l.length(), -10*(l.dx()+l.dy())/l.length());
		painter.drawLine(start, end);
	}
}


//print pressure and velocity over time (only in debug mode)
void InputWidget::printData()
{
#ifndef NO_DEBUG
	if(path.empty())
		return;
	QTime start = path.at(0).time;
	double length = 0;
	for(int i = 0; i< path.count(); i++) {
		if(i > 0)
			length = QLineF(path.at(i-1).pos, path.at(i).pos).length();
		double time = start.msecsTo(path.at(i).time);
		std::cout << time << "\t";
		std::cout << path.at(i).pressure << "\t";
		std::cout << length/time << "\n";
	}
#endif
}


//used while recording, takes last arrow and removes all Nodes belonging to it
void InputWidget::deleteLastStroke()
{
	if(arrows.isEmpty())
		return;

	Arrow a = arrows.takeLast();
	while(a.start_node_id+1 < path.count())
		path.removeAt(a.start_node_id+1);

	update();
}
