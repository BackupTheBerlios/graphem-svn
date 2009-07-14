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
#include "graphem.h"
#include "newpattern.h"

#include <iostream>

#include <QCoreApplication>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QTextEdit>
#include <QWidget>


Graphem::Graphem(Auth *auth):
	auth(auth),
	info_text(0)
{
	settings = new QSettings();

	usage_total = settings->value("usage_total").toInt();
	usage_failed = settings->value("usage_failed").toInt();

	setWindowTitle(graphem_version);
	setCentralWidget(input);

	//menu bar
	QMenu *file = menuBar()->addMenu(tr("&File")); //naming?
	file->addAction(tr("&New Pattern..."), this,
		SLOT(showNewPatternDialog()), tr("Ctrl+N"));
	file->addSeparator();
	file->addAction(tr("&Quit"), this, SLOT(quit()), tr("Ctrl+Q"));
	menuBar()->addMenu(tr("&Settings"));
		// show input
		// processing timeout
	QMenu *help = menuBar()->addMenu(tr("&Help"));
	help->addAction(tr("&About Qt"), this, SLOT(aboutQt()), 0);
	//about
	
	//info dock
	info_text = new QTextEdit();
	info_text->setReadOnly(true);
	refreshInfo();
	QDockWidget *info_dock = new QDockWidget(tr("Information"), this);
	info_dock->setWidget(info_text);
	addDockWidget(Qt::LeftDockWidgetArea, info_dock);

	resize(600,400);

	connect(input, SIGNAL(dataReady()),
		this, SLOT(authenticate()),
		Qt::QueuedConnection); //allow for repaint before checking

/* TODO
	connect(auth, SIGNAL(failed()),
		this, SLOT(failed()));
	connect(auth, SIGNAL(passed()),
		this, SLOT(passed()));
	*/
}


void Graphem::failed()
{
	usage_total++;
	usage_failed++;
	if(tries_left == 1) //this try was our last
		exit(1);

	if(tries_left != 0) //0 is for infinite amount of tries, don't decrease
		tries_left--;

	input->reset();
	if(!lock_screen)
			refreshInfo();
}


void Graphem::passed()
{
	usage_total++;
	if(lock_screen) {
		quit();
		return;
	}

	input->reset();
	refreshInfo();
}


void Graphem::authenticate()
{
	auth->preprocess(input->path);
	if(print_pattern)
		auth->printPattern();
	auth->check();
}


//refreshes info text on left side
void Graphem::refreshInfo()
{
	if(!auth->loadHash()) {
		info_text->setText(tr("<h3>Welcome</h3>To start using Graphem, you have to set a new authentication pattern. Please click the \"New Pattern\" button.<br />You can find a tutorial at <a href='http://graphem.berlios.de/'>http://graphem.berlios.de/</a>"));
		input->showMessage("");
		input->setEnabled(false);
	} else {
		info_text->setText(tr("<h3>Statistics</h3>\
			Total: %1 <br />\
			Correct: %2 / Failed: %3 <br />\
			Recognition Rate: %4\%")
			.arg(usage_total)
			.arg(usage_total-usage_failed)
			.arg(usage_failed)
			.arg(qRound(double(usage_total - usage_failed)/usage_total*1000)/10.0));
		input->setEnabled(true);
		input->setDefaultMessage(tr("You can test your pattern here."));
		input->showMessage();
	}
}


void Graphem::resetStats()
{
	usage_total = 0;
	usage_failed = 0;
	refreshInfo();
}


void Graphem::showNewPatternDialog()
{
	QMessageBox msgBox(QMessageBox::Question,
		tr("Enable touchpad mode?"),
		tr("<b>Enable touchpad mode?</b>"),
		QMessageBox::Cancel,
		this);
	msgBox.setInformativeText(tr("Enable this if you want to use mouse movements without clicking. When recording, you will still need to hold your mouse button down, but no \"pen up\" events will be stored."));
	msgBox.addButton(tr("&Enable"), QMessageBox::YesRole);
	msgBox.setDefaultButton(msgBox.addButton(tr("Use &Normal Mode"), QMessageBox::NoRole));
	int ret = msgBox.exec();

	if(ret == QMessageBox::Cancel)
		return;
	
	//return value doesn't seem to be QMessageBox::Yes for enabling.. ??
	NewPattern *new_pattern_dialog = new NewPattern(this, !ret);
	if(new_pattern_dialog->exec() == QDialog::Accepted)
		resetStats();
	delete new_pattern_dialog;
}


void Graphem::quit()
{
	settings->setValue("usage_total", usage_total);
	settings->setValue("usage_failed", usage_failed);
	settings->sync();

	qApp->quit();
}
