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
#include <QLineF>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

NewPattern::NewPattern(QWidget *parent, bool touchpad_mode):
	QDialog(parent),
	input(new InputWidget(this, true)), //InputWidget in record mode
	touchpad_mode(touchpad_mode)
{
	resize(600,400);
	setWindowTitle("New Pattern");

	input->enableTouchpadMode(touchpad_mode);
	input->setDefaultMessage("");
	input->showMessage("Enter your new pattern here.", 3000);
	
	QDialogButtonBox *button_box = new QDialogButtonBox(this);
	button_box->addButton("&Cancel", QDialogButtonBox::RejectRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(reject()));
	button_box->addButton("&Save", QDialogButtonBox::AcceptRole);
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(save()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(accept()));
	QPushButton *delete_last = button_box->addButton("&Delete Last Stroke", QDialogButtonBox::ActionRole);
	connect(delete_last, SIGNAL(clicked()),
		input, SLOT(deleteLastStroke()));
	QPushButton *reset = button_box->addButton("&Reset", QDialogButtonBox::ResetRole);
	connect(reset, SIGNAL(clicked()),
		input, SLOT(reset()));
	
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->addWidget(input);
	l1->addWidget(button_box);
	setLayout(l1);

	connect(input, SIGNAL(dataReady()),
		this, SLOT(updateDisplay()));
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

	setWindowTitle(tr("New Pattern (%1 Stroke(s))").arg(auth.strokes.count()));
}


//save pattern to config file
void NewPattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);

	QSettings* settings = new QSettings();
	QByteArray salt = generateSalt();
	settings->setValue("pattern_hash", getHash(auth.strokesToString(), salt));
	settings->setValue("touchpad_mode", touchpad_mode);
	settings->setValue("salt", salt);
	settings->sync();
	delete settings;
}
