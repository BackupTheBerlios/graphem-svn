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

#include <QCryptographicHash>
#include <QDialogButtonBox>
#include <QLineF>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

NewPattern::NewPattern(QWidget *parent):
	QWidget(parent),
	input(new InputWidget(this, true)) //InputWidget in record mode
{
	setWindowModality(Qt::ApplicationModal);
	resize(600,400);
	setWindowTitle("New Pattern");
	
	QDialogButtonBox *button_box = new QDialogButtonBox(this);
	button_box->addButton("&Cancel", QDialogButtonBox::RejectRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(close()));
	button_box->addButton("&Save", QDialogButtonBox::AcceptRole);
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(save()));
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
		this, SLOT(drawArrows()));
}


void NewPattern::drawArrows()
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
}


//save pattern to config file
void NewPattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);

	QSettings* settings = new QSettings();
	settings->setValue("pattern_hash", QCryptographicHash::hash(auth.strokesToString().toAscii(), QCryptographicHash::Sha1));
	settings->sync();
	delete settings;
}
