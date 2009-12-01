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

#include "graphem.h"
#include "preferences.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSettings>
#include <QSpinBox>

#ifdef Q_WS_X11
#include <QX11Info>
#endif

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

	QLabel *check_timeout_label = new QLabel(tr("&Authentication timeout"));
	check_timeout = new QSpinBox();
	check_timeout->setRange(1, 120);
	check_timeout->setSuffix("s");
	check_timeout->setValue(settings.value("check_timeout", CHECK_TIMEOUT).toInt());
	check_timeout_label->setBuddy(check_timeout);

	show_input = new QCheckBox(tr("&Show input when drawing"));
	if(settings.value("show_input", SHOW_INPUT).toBool())
		show_input->setCheckState(Qt::Checked);

#if defined Q_WS_X11 && QT_VERSION > 0x040400
	bool enable_fading = x11Info().isCompositingManagerRunning();
#else
	bool enable_fading = true; //we'll just assume it works
#endif

	fade = new QCheckBox(tr("&Fade in when locking screen"));
	fade->setChecked(enable_fading and settings.value("fade", FADE).toBool());
	fade->setEnabled(enable_fading);

	QLabel *window_opacity_label = new QLabel(tr("Final window &opacity"));
	window_opacity = new QDoubleSpinBox();
	window_opacity->setRange(0.0, 1.0);
	window_opacity->setValue(settings.value("window_opacity", WINDOW_OPACITY).toDouble());
	window_opacity->setSingleStep(0.1);
	window_opacity_label->setBuddy(window_opacity);
	window_opacity->setEnabled(enable_fading);

	//layout
	QVBoxLayout *l1 = new QVBoxLayout();

	QHBoxLayout *l2 = new QHBoxLayout();
	l2->addWidget(check_timeout_label);
	l2->addWidget(check_timeout);
	l1->addLayout(l2);

	l1->addWidget(show_input);

	l1->addWidget(fade);
	QHBoxLayout *l3 = new QHBoxLayout();
	l3->addWidget(window_opacity_label);
	l3->addWidget(window_opacity);
	l1->addLayout(l3);

	l1->addWidget(button_box);

	setLayout(l1);
}


//save and accept dialog
void Preferences::save()
{
	QSettings settings;
	settings.setValue("show_input", show_input->isChecked());
	settings.setValue("fade", fade->isChecked());
	settings.setValue("check_timeout", check_timeout->value());
	settings.setValue("window_opacity", window_opacity->value());
	settings.sync();

	accept();
}
