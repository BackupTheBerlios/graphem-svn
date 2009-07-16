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
#include "generatepattern.h"
#include "inputwidget.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

GeneratePattern::GeneratePattern(QWidget *parent):
	QDialog(parent),
	input(new InputWidget(this))
{
	setWindowTitle(tr("Generate new key pattern"));
	input->setEnabled(false); //TODO if this is sufficient, delete InputWidget::enableInput()
	input->setDefaultMessage("");
	input->showMessage("");

	QLabel *strokes_count_label = new QLabel(tr("&Number of strokes"));
	strokes_count = new QSpinBox(this);
	strokes_count_label->setBuddy(strokes_count);

	touchpad = new QCheckBox(tr("Use &touchpad mode"));

	QPushButton *generate_button = new QPushButton(tr("&Generate"));
	connect(generatepattern, SIGNAL(clicked()),
		this, SLOT(generate()));


	// layout
	QHBoxLayout *lh = new QHBoxLayout();
	QVBoxLayout *lv = new QVBoxLayout();
	QHBoxLayout *lstrokes = new QHBoxLayout();
	lstrokes->addWidget(strokes_count_label);
	lstrokes->addWidget(strokes_count);
	lv->addLayout(lstrokes);
	lv->addWidget(touchpad);
	lh->addLayout(lv);
	lh->addWidget(input);

	setLayout(lh);
}


//generate new key and display
void GeneratePattern::generate()
{
	//TODO
}


//save pattern to config file
void GeneratePattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);
	auth.saveHash();
}
