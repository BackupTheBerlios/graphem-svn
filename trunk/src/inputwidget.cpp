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

#include "graphem.h"
#include "inputwidget.h"

#include <iostream>

#include <QApplication>
#include <QCursor>
#include <QLineF>
#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QTime>
#include <QTimer>


InputWidget::InputWidget(QWidget* parent, bool record) :
	QWidget(parent),
	pen_down(false),
	mouse_down(false),
	timer(new QTimer(this)),
	msg_timer(new QTimer(this)),
	msg(""),
	default_msg(tr("Please enter your key pattern.")),
	touchpad_mode(false),
	show_input(false),
	record_pattern(record),
	cursor_centered(false),
	do_grab(false)
{
	setMinimumSize(300,200);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	msg_timer->setSingleShot(true);
	connect(msg_timer, SIGNAL(timeout()),
		this, SLOT(showMessage()));

	QSettings settings;
	fade_to = settings.value("window_opacity", WINDOW_OPACITY).toDouble();
	timer->setInterval(70); //check every 70ms
	if(record_pattern) {
		connect(timer, SIGNAL(timeout()),
			this, SIGNAL(dataReady()));
		reset();
	} else {
		connect(timer, SIGNAL(timeout()),
			this, SLOT(checkFinished()));
		show_input = settings.value("show_input", SHOW_INPUT).toBool();
	}
	showMessage();
	timer->start();
}


void InputWidget::checkFinished()
{
	if(path.isEmpty() or path.count() == 1) //we need at least 2 events
		return;
	showMessage(""); //don't show message after input started

	if(path.last().time.secsTo(QTime::currentTime()) >= 1 //wait 1s after last input
	or path.count() >= 100000) { //don't crash if input device generates too much data
		bool tmp = show_input;
		show_input = false; //don't show input for extended periods of time while processing

		showMessage(tr("Processing..."));
		repaint();

		show_input = tmp;
		emit dataReady();
	}
}


void InputWidget::enableTouchpadMode(bool on)
{
	touchpad_mode = on;
	if(!record_pattern)
		setMouseTracking(touchpad_mode);
}


//fade-in effect
void InputWidget::fade()
{
	if(windowOpacity() < fade_to) {
		qreal fade_step = fade_to/(FADE_TIME/FADE_STEP_TIME);
		setWindowOpacity(windowOpacity() + fade_step);
		QTimer::singleShot(FADE_STEP_TIME, this, SLOT(fade()));
	} else { // end fade-in
		setWindowOpacity(fade_to);
	}
}


// get mouse/keyboard grab and focus; needs to be called after window is displayed
void InputWidget::focus()
{
	if(do_grab) {
		grabMouse();
		grabKeyboard();
	}

	activateWindow(); //make sure we catch keyboard events
	raise();
}


void InputWidget::showEvent(QShowEvent*)
{
	if(parent() == 0)
		focus();

	//start fade-in if in LOCK-mode
	if(do_grab) {
		QSettings settings;
		if(settings.value("fade", FADE).toBool()) {
			setWindowOpacity(0.0);
			QTimer::singleShot(FADE_STEP_TIME, this, SLOT(fade()));
		} else {
			setWindowOpacity(fade_to);
		}
	}

	//when called via keyboard shortcut, keyboard grab might fail
	//after 500ms, the key should have been released so the grab can work
	QTimer::singleShot(500, this, SLOT(focus()));
}


void InputWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if(cursor_centered) { //drop event so we actually start at center
		cursor_centered = false;
		return;
	}
	/* TODO: uncomment me when we're processing tablet events
	if(pen_down) { //ignore mouse events while pen is down
		ev->ignore();
		return;
	}*/

	if(!touchpad_mode) {
		if(ev->buttons() == 0) { //no buttons pressed
			mouse_down = false;
			return;
		}
		mouse_down = true;
	}

	path.append(Node(ev->pos()));
	update();
}


void InputWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if(pen_down or !mouse_down or touchpad_mode)
		return;

	mouse_down = false;

	path.append(Node(ev->pos(), true));
	update();
}


void InputWidget::paintEvent(QPaintEvent* /*ev*/)
{
	QPainter painter(this);
	painter.setPen(Qt::white);
	painter.fillRect(rect(), QBrush(Qt::black));

	painter.drawText(width()/2 - 100, height()/2, msg);
	if(touchpad_mode and !record_pattern)
		painter.drawText(11, 19, tr("[Touchpad Mode]"));

	if(show_input and !path.isEmpty()) {
		QPainterPath painter_path;
		painter.setPen(Qt::red);

		for(int i=0; i < path.count(); i++) {
			if(path.at(i).pen_up)
				continue;
			if(i == 0 or path.at(i-1).pen_up)
				painter_path.moveTo(path.at(i).pos);
			else
				painter_path.lineTo(path.at(i).pos);
			painter.drawEllipse(path.at(i).pos.x()-1, path.at(i).pos.y()-1, 2, 2);
		}
		painter.setPen(Qt::blue);
		painter.drawPath(painter_path);
	}

	emit redraw(&painter);
}


//print  pen_up(pressure) and velocity over time (only in debug mode)
void InputWidget::printData()
{
#ifndef QT_NO_DEBUG
	if(path.empty())
		return;
	std::cout << "t\tp(t)\tv(t)\n";
	QTime start = path.at(0).time;
	double length = 0;
	for(int i = 0; i< path.count(); i++) {
		if(i > 0)
			length = QLineF(path.at(i-1).pos, path.at(i).pos).length();
		double time = start.msecsTo(path.at(i).time);
		std::cout << time << "\t";
		std::cout << int(path.at(i).pen_up) << "\t";
		std::cout << length/time << "\n";
	}
#endif
}


void InputWidget::reset()
{
	path.clear();
	QSettings settings;
	show_input = settings.value("show_input", SHOW_INPUT).toBool();
	if(!record_pattern)
		showMessage(tr("Pattern not recognized, please try again."), 1500);
	update();
}

void InputWidget::resizeEvent(QResizeEvent* /*ev*/)
{
	if(parent() == 0) { //center cursor for top level windows
		cursor().setPos(width()/2, height()/2);
		cursor_centered = true;
	}
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
