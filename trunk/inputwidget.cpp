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

#include <iostream>

#include <QMouseEvent>
#include <QPainter>
#include <QTime>
#include <QTimer>


InputWidget::InputWidget(QWidget* parent) :
	QWidget(parent),
	pen_down(false),
	mouse_down(false),
	timer(new QTimer(this)),
	msg(""),
	touchpad_mode(false),
	show_input(false)
{
	setMinimumSize(300,200);
	timer->setInterval(200); //check every 0.2s
	connect(timer, SIGNAL(timeout()),
		this, SLOT(checkFinished()));
	timer->start();
	showMessage();
}


void InputWidget::checkFinished()
{
	if(path.isEmpty() or path.count() == 1) //we need at least 2 events
		return;
	showMessage(""); //don't show message after input started

	if(path.last().time.secsTo(QTime::currentTime()) >= 1) {
		showMessage("Processing...");
		repaint();
		emit finished();
	}
}


void InputWidget::reset()
{
	path.clear();
	showMessage("Pattern not recognized, please try again.", 1500);
}


void InputWidget::showInput(bool b) { show_input = b; }


void InputWidget::showMessage(QString m, int msecs)
{
	if(msg != m)
		update(); //repaints after returning to event loop
	msg = m;
	if(msecs)
		QTimer::singleShot(msecs, this, SLOT(showMessage()));
}


void InputWidget::enableTouchpadMode(bool b) { setMouseTracking(touchpad_mode = b); }


void InputWidget::mousePressEvent(QMouseEvent* ev)
{
	if(pen_down or touchpad_mode) //ignore mouse events while we're already getting tablet data
		return;

	mouse_down = true;

	path.append(Node(ev->pos(), QTime::currentTime()));
}


void InputWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if(pen_down or !mouse_down or touchpad_mode)
		return;

	mouse_down = false;

	path.append(Node(ev->pos(), QTime::currentTime()));
	path.append(Node::makeSeparator());
	update();
}


void InputWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(pen_down) {
		ev->ignore();
		return;
	}

	if(mouse_down or touchpad_mode) {
		path.append(Node(ev->pos(), QTime::currentTime()));
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
	if(!show_input or path.isEmpty())
		return; //nothing to paint

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


//print pressure and velocity over time
void InputWidget::printData()
{
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
}
