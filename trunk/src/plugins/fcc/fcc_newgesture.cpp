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

// TODO: add minimal stroke length

#include "crypto.h"
#include "fcc_auth.h"
#include "inputwidget.h"
#include "fcc_newgesture.h"

#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLineF>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <QtDebug>


FCCNewGesture::FCCNewGesture(QWidget *parent, Auth *auth):
	NewGesture(parent, auth),
	input(new InputWidget(this, true)) //InputWidget in record mode
{
	QMessageBox msgBox(QMessageBox::Question, "", tr("<b>Enable touchpad mode?</b>"), QMessageBox::NoButton, this);
	msgBox.setInformativeText(tr("Enable this if you want to use mouse movements without clicking. When recording, you will still need to hold your mouse button down, but no \"pen up\" events will be stored."));
	msgBox.addButton(tr("&Enable"), QMessageBox::YesRole);
	msgBox.setDefaultButton(msgBox.addButton(tr("Use &Normal Mode"), QMessageBox::NoRole));
	const int ret = msgBox.exec();

	//return value doesn't seem to be QMessageBox::Yes for enabling.. ??
	touchpad_mode = !ret;

	resize(600,400);
	setWindowTitle(tr("New Gesture"));

	input->enableTouchpadMode(touchpad_mode);
	input->setDefaultMessage("");
	input->showMessage(tr("Enter your new gesture here."), 3000);
	
	connect(input, SIGNAL(dataReady()),
		this, SLOT(updateDisplay()));
	connect(input, SIGNAL(redraw(QPainter*)),
		this, SLOT(draw(QPainter*)));

	QDialogButtonBox *button_box = new QDialogButtonBox(this);
	button_box->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
	button_box->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
	QPushButton *delete_last = button_box->addButton(tr("&Delete Last Stroke"), QDialogButtonBox::ActionRole);
	QPushButton *reset = button_box->addButton(tr("&Reset"), QDialogButtonBox::ResetRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(reject()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(accept()));
	connect(delete_last, SIGNAL(clicked()),
		this, SLOT(deleteLastStroke()));
	connect(reset, SIGNAL(clicked()),
		this, SLOT(resetInput()));
	
#ifndef NO_QCA
	QPushButton *generate = button_box->addButton(tr("&Generate"), QDialogButtonBox::ActionRole);
	connect(generate, SIGNAL(clicked()),
		this, SLOT(generate()));
#endif

	QVBoxLayout *l1 = new QVBoxLayout();
	l1->setMargin(0);
	button_box->setContentsMargins(9, 0, 9, 0);
	l1->addWidget(input);
	l1->addWidget(button_box);
	setLayout(l1);
}


//used while recording, takes last arrow and removes all Nodes belonging to it
void FCCNewGesture::deleteLastStroke()
{
	if(arrows.isEmpty())
		return;

	Arrow a = arrows.takeLast();
	while(a.start_node+1 < input->path.count())
		input->path.removeAt(a.start_node+1);

	input->update();
}


// draw arrows over InputWidget
void FCCNewGesture::draw(QPainter *painter)
{
	//approximation to 3*cos() to get rid of floating point errors
	const int x[] = { 3, 2, 0, -2, -3, -2, 0, 2 };

	for(int i=0; i < arrows.count(); i++) {
		if(arrows.at(i).pen_up)
			painter->setPen(Qt::red);
		else
			painter->setPen(Qt::white);

		const int dir = arrows.at(i).direction;
		const QPointF start = input->path.at(arrows.at(i).start_node).pos;
		const QPointF end = start + arrows.at(i).weight * QPoint(x[dir], x[(dir+2)%8])/3;
		const QLineF l(start, end);
		if(l.length() == 0)
				continue;

		painter->drawLine(l);

		QPointF a  = end + QPointF(-10*(l.dy()+l.dx())/l.length(), 10*(l.dx()-l.dy())/l.length());
		painter->drawLine(a, end);

		//print number at start
		//painter->drawText(start+end-a, QString::number(i));

		a = end + QPointF(10*(l.dy()-l.dx())/l.length(), -10*(l.dx()+l.dy())/l.length());
		painter->drawLine(a, end);
	}

	painter->setPen(Qt::white);
	painter->drawText(11, 19,(tr("%1 Stroke(s)").arg(arrows.count())));
}


//generate random key and display
void FCCNewGesture::generate()
{
#ifndef NO_QCA
	bool ok;
	const int num_strokes_start = 10;
	const int num_strokes_min = 1;
	const int num_strokes_max = 100;
	const int num_strokes = QInputDialog::getInteger(this, tr("Generate Random Gesture"),
		(input->path.empty()?tr(""):tr("Warning: The current input will be erased!<br>"))+tr("Number of strokes:"),
		num_strokes_start, num_strokes_min, num_strokes_max, 1, &ok);
	if(!ok)
		return;
	resetInput();

	bool last_pen_up = true;
	bool pen_up;
	int last_x = 0; int last_y = 0;
	int x, y;
	QPoint lastpos = QPoint(Crypto::randInt(0, input->width()),
			Crypto::randInt(0, input->height()));
	input->path.append(Node(lastpos)); //start here 
	for(int i = 0; i < num_strokes; i++) {
		const int l = Crypto::randInt(30, 250);
		x = Crypto::randInt(-1, 1);
		y = Crypto::randInt(-1, 1);
		//add offset to avoid having strokes directly on top of each other
		QPoint pos = lastpos + l*QPoint(x, y) + 5*QPoint(y, -x);

		if(touchpad_mode or last_pen_up or i >= num_strokes-2) {
			pen_up = false;
		} else { //insert ~20% "up"-strokes
			pen_up = (Crypto::randInt(0, 100) <= 20);
		}

		if(!input->rect().contains(pos, true) //outside input widget
		or (last_x == x and last_y == y and !last_pen_up) //same direction as last stroke
		or	QLineF(pos, lastpos).length() < 20 ) { //too short
			i--;
			continue;
		}

		input->path.append(Node(pos, pen_up));

		last_x = x; last_y = y;
		lastpos = pos;
		last_pen_up = pen_up;
	}
#endif
}


//reset input and re-enable touchpad mode if required
void FCCNewGesture::resetInput()
{
	arrows.clear();
	input->reset();
	input->enableTouchpadMode(touchpad_mode);
}


void FCCNewGesture::save()
{
	FCC tmpauth;
	tmpauth.setInput(input);
	input->enableTouchpadMode(touchpad_mode);
	tmpauth.preprocess();

	QSettings settings;
	QByteArray salt = Crypto::generateSalt();
	settings.setValue("pattern_hash", Crypto::getHash(tmpauth.strokesToString(), salt));
	settings.setValue("touchpad_mode", touchpad_mode);
	settings.setValue("salt", salt);
	settings.setValue("usage_total", 0);
	settings.setValue("usage_failed", 0);
	settings.sync();

	qDebug() << "saved gesture " << tmpauth.strokesToString();
}


void FCCNewGesture::updateDisplay()
{
	arrows.clear();
	FCC tmpauth;
	tmpauth.setInput(input); //this might change input->touchpad_mode
	input->enableTouchpadMode(touchpad_mode);
	tmpauth.preprocess();

	for(int i = 0; i < tmpauth.strokes.count(); i++) {
		Arrow a;
		a.start_node = tmpauth.strokes.at(i).start_node_id;
		a.pen_up = tmpauth.strokes.at(i).up;
		a.direction = tmpauth.strokes.at(i).direction;
		a.weight = int(tmpauth.strokes.at(i).length);
		arrows.append(a);
	}

	input->update();
}
