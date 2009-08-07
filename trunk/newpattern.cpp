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

#include "auth.h"
#include "crypto.h"
#include "inputwidget.h"
#include "newpattern.h"

#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLineF>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QVBoxLayout>

NewPattern::NewPattern(QWidget *parent, bool touchpad_mode):
	QDialog(parent),
	input(new InputWidget(this, true)), //InputWidget in record mode
	status(new QStatusBar(this))
{
	resize(600,400);
	setWindowTitle(tr("New Pattern"));

	input->enableTouchpadMode(touchpad_mode);
	input->setDefaultMessage("");
	input->showMessage(tr("Enter your new pattern here."), 3000);
	
	connect(input, SIGNAL(dataReady()),
		this, SLOT(updateDisplay()));

	QDialogButtonBox *button_box = new QDialogButtonBox(this);
	button_box->addButton(tr("&Cancel"), QDialogButtonBox::RejectRole);
	button_box->addButton(tr("&Ok"), QDialogButtonBox::AcceptRole);
	QPushButton *generate = button_box->addButton(tr("&Generate Pattern"), QDialogButtonBox::ActionRole);
	QPushButton *delete_last = button_box->addButton(tr("&Delete Last Stroke"), QDialogButtonBox::ActionRole);
	QPushButton *reset = button_box->addButton(tr("&Reset"), QDialogButtonBox::ResetRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(reject()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(accept()));
	connect(generate, SIGNAL(clicked()),
		this, SLOT(generate()));
	connect(delete_last, SIGNAL(clicked()),
		input, SLOT(deleteLastStroke()));
	connect(reset, SIGNAL(clicked()),
		input, SLOT(reset()));
	
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->setMargin(0);
	button_box->setContentsMargins(9, 0, 9, 0);
	l1->addWidget(input);
	l1->addWidget(button_box);
	l1->addWidget(status);
	setLayout(l1);
}


//generate random key and display
void NewPattern::generate()
{
	bool ok;
	const int num_strokes = QInputDialog::getInteger(this, tr("Generate Random Pattern"),
		(input->path.empty()?tr(""):tr("Warning: The current input will be erased!<br>"))+tr("Number of strokes:"),
		8, 0, 10000, 1, &ok);
	if(!ok)
		return;
	input->reset();

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
		QPoint pos = lastpos + l*QPoint(x, y) + 5*QPoint(y, -x);
		//last summand is offset to avoid having strokes directly on top of each other

		if(input->touchpad_mode or last_pen_up or i >= num_strokes-2) {
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
}


void NewPattern::updateDisplay()
{
	input->arrows.clear();
	Auth auth(this);
	auth.preprocess(input->path);

	for(int i = 0; i < auth.strokes.count(); i++) {
		Arrow a;
		a.start_node_id = auth.strokes.at(i).start_node_id;
		a.pen_up = auth.strokes.at(i).up;
		a.direction = auth.strokes.at(i).direction;
		a.weight = auth.strokes.at(i).length;
		input->arrows.append(a);
	}
	input->update();

	status->showMessage(tr("%1 Stroke(s)").arg(auth.strokes.count()));
}


void NewPattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);
	auth.saveHash();
}
