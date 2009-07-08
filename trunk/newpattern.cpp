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

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

NewPattern::NewPattern(QWidget *parent):
	QWidget(parent)
	//input(new InputWidget(this))
{
		setWindowModality(Qt::ApplicationModal);
		setWindowTitle("New Pattern");

		cancel = new QPushButton("&Cancel");
		connect(cancel, SIGNAL(clicked()),
			this, SLOT(close()));
		save_pattern = new QPushButton("&Save");
		delete_last = new QPushButton("&Delete Last Stroke");
		clear_pattern = new QPushButton("&Clear");

		QHBoxLayout *l2 = new QHBoxLayout();
		l2->addWidget(delete_last);
		l2->addWidget(clear_pattern);
		l2->addWidget(save_pattern);
		l2->addWidget(cancel);

		QVBoxLayout *l1 = new QVBoxLayout();
		//l1->addWidget(input);
		l1->addLayout(l2);
		setLayout(l1);
}
