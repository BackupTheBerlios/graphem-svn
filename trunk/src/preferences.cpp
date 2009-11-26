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
#include <QX11Info>

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
	if(settings.value("show_input", false).toBool())
		show_input->setCheckState(Qt::Checked);

#if defined Q_WS_X11 && QT_VERSION > 0x040400
	bool enable_fading = x11Info().isCompositingManagerRunning();
#else
	bool enable_fading = true; //we'll just assume it works
#endif

	fade = new QCheckBox(tr("&Fade in when locking screen"));
	if(enable_fading and settings.value("fade", true).toBool())
		fade->setCheckState(Qt::Checked);
	fade->setEnabled(enable_fading);

	QLabel *check_timeout_label = new QLabel(tr("&Authentication timeout"));
	check_timeout = new QSpinBox();
	check_timeout->setRange(1, 120);
	check_timeout->setSuffix("s");
	check_timeout->setValue(settings.value("check_timeout", 6).toInt());
	check_timeout_label->setBuddy(check_timeout);
	
	//layout
	QVBoxLayout *l1 = new QVBoxLayout();
	l1->addWidget(show_input);
	l1->addWidget(fade);
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
	settings.setValue("fade", fade->checkState() == Qt::Checked);
	settings.setValue("check_timeout", check_timeout->value());
	settings.sync();

	accept();
}
