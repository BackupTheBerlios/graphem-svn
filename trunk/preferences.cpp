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

#include "preferences.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QSettings>
#include <QSpinBox>

Preferences::Preferences(QWidget *parent):
	QDialog(parent)
{
	setWindowTitle(tr("Preferences"));
	QSettings settings;

	//set up controls
	QDialogButtonBox *button_box = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	connect(button_box, SIGNAL(rejected()),
		this, SLOT(reject()));
	connect(button_box, SIGNAL(accepted()),
		this, SLOT(save()));

	show_input = new QCheckBox(tr("&Show input when drawing"));
	if(settings.value("show_input").toBool())
		show_input->setCheckState(Qt::Checked);

	QLabel *check_timeout_label = new QLabel(tr("&Authentication timeout"));
	check_timeout = new QSpinBox();
	check_timeout->setRange(1, 120);
	check_timeout->setSuffix("s");
	check_timeout->setValue(settings.value("check_timeout", 6).toInt());
	check_timeout_label->setBuddy(check_timeout);
	
	//layout
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->addWidget(show_input);
	QHBoxLayout *l2 = new QHBoxLayout();
	l2->addWidget(check_timeout_label);
	l2->addWidget(check_timeout);
	l1->addLayout(l2);
	l1->addWidget(button_box);

	setLayout(l1);
}


//save and accept dialog
void Preferences::save()
{
	QSettings settings;
	settings.setValue("show_input", show_input->checkState() == Qt::Checked);
	settings.setValue("check_timeout", check_timeout->value());
	settings.sync();

	accept();
}
