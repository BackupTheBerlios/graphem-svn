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
#include "generatepattern.h"
#include "inputwidget.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

GeneratePattern::GeneratePattern(QWidget *parent):
	QDialog(parent),
	input(new InputWidget(this, true))
{
	setWindowTitle(tr("Generate new key pattern"));
	input->setEnabled(false); //TODO if this is sufficient, delete InputWidget::enableInput()
	input->setDefaultMessage("");
	input->showMessage("");

	QLabel *strokes_count_label = new QLabel(tr("&Number of strokes"));
	strokes_count = new QSpinBox(this);
	strokes_count->setValue(8);
	strokes_count_label->setBuddy(strokes_count);

	touchpad = new QCheckBox(tr("Use &touchpad mode"));

	QPushButton *generate_button = new QPushButton(tr("&Generate"));
	connect(generate_button, SIGNAL(clicked()),
		this, SLOT(generate()));


	// layout
	QHBoxLayout *lh = new QHBoxLayout();
	QVBoxLayout *lv = new QVBoxLayout();
	QHBoxLayout *lstrokes = new QHBoxLayout();
	lstrokes->addWidget(strokes_count_label);
	lstrokes->addWidget(strokes_count);
	lv->addLayout(lstrokes);
	lv->addWidget(touchpad);
	lv->addWidget(generate_button);
	lh->addLayout(lv);
	lh->addWidget(input);

	setLayout(lh);
}


//generate new key and display
void GeneratePattern::generate()
{
	// TODO create corresponding hash
	input->arrows.clear();

	bool last_pen_up = true;
	bool pen_up;
	QPoint lastpos = QPoint(Crypto::randInt(0, input->width()),
			Crypto::randInt(0, input->height()));
	for(int i = 0; i < strokes_count->value(); i++) {
		QPoint pos = lastpos + QPoint(Crypto::randInt(-150, 150),
			Crypto::randInt(-150, 150));
		if(!input->rect().contains(pos, true)) {
			// outside input widget, try again
			i--;
			continue;
		}

		if(touchpad->checkState() == Qt::Checked or last_pen_up) {
			pen_up = false;
		} else { //insert ~20% "up"-strokes
			pen_up = (Crypto::randInt(0, 100) <= 20);
			pen_up = true; //TODO just testing
			last_pen_up = pen_up;
		}

		if(pen_up) {
			//TODO broken
			input->path.append(Node(pos));
			input->path.append(Node::makeSeparator(pos));
			input->path.append(Node::makeSeparator(pos));
		} else {
			input->path.append(Node(pos));
		}
		lastpos = pos;
	}

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
}


//save pattern to config file
void GeneratePattern::save()
{
	Auth auth(this);
	auth.preprocess(input->path);
	auth.saveHash();
}
