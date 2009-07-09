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
#include "newpattern.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

NewPattern::NewPattern(QWidget *parent):
	QWidget(parent),
	input(new InputWidget(this))
{
	setWindowModality(Qt::ApplicationModal);
	resize(600,400);
	setWindowTitle("New Pattern");
	
	input->showInput(true);
	input->showMessage("Draw your new pattern here.");
	
	QDialogButtonBox *button_box = new QDialogButtonBox(this);
	button_box->addButton("&Cancel", QDialogButtonBox::RejectRole);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(close()));
	button_box->addButton("&Save", QDialogButtonBox::AcceptRole);
	button_box->addButton("&Delete Last Stroke", QDialogButtonBox::ActionRole);
	button_box->addButton("&Reset", QDialogButtonBox::ResetRole);
	
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->addWidget(input);
	l1->addWidget(button_box);
	setLayout(l1);
}
