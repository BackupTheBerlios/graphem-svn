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
#include "inputwidget.h"
#include "newpattern.h"

#include <QDialogButtonBox>
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
	button_box->addButton(tr("&Save"), QDialogButtonBox::AcceptRole);
	QPushButton *delete_last = button_box->addButton(tr("&Delete Last Stroke"), QDialogButtonBox::ActionRole);
	QPushButton *reset = button_box->addButton(tr("&Reset"), QDialogButtonBox::ResetRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(reject()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(save()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(accept()));
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

	status->showMessage(tr("%1 Stroke(s)").arg(auth.strokes.count()));
}


//save pattern to config file
void NewPattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);
	auth.saveHash();
}
