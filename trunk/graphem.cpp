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
#include "generatepattern.h"
#include "graphem.h"
#include "newpattern.h"
#include "preferences.h"

#include <QCoreApplication>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>
#include <QTextEdit>


Graphem::Graphem(InputWidget* input):
	input(input),
	info_text(0)
{

	setWindowTitle(graphem_version);
	setCentralWidget(input);

	//menu bar
	QMenu *file = menuBar()->addMenu(tr("&File"));
	file->addAction(tr("&New Pattern..."), this,
		SLOT(showNewPatternDialog()), tr("Ctrl+N"));
	file->addAction(tr("&Generate Random Pattern..."), this,
		SLOT(showGeneratePatternDialog()), tr("Ctrl+G"));
	file->addSeparator();
	file->addAction(tr("&Preferences"), this,
		SLOT(showPreferences()), tr("Ctrl+P"));
	file->addSeparator();
	file->addAction(tr("&Quit"), this, SLOT(quit()), tr("Ctrl+Q"));

		// processing timeout
	QMenu *help = menuBar()->addMenu(tr("&Help"));
	help->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()), 0);
	help->addAction(tr("&About"), this, SLOT(showAboutDialog()), 0);
	
	//info dock
	info_text = new QTextEdit();
	info_text->setReadOnly(true);
	info_dock = new QDockWidget(this);
	info_dock->setWidget(info_text);
	addDockWidget(Qt::LeftDockWidgetArea, info_dock);

	resize(600,400);

	connect(input->auth(), SIGNAL(passed()),
		this, SLOT(reset()));
	connect(input->auth(), SIGNAL(failed()),
		this, SLOT(reset()));
		
	refreshInfo();
}


void Graphem::reset()
{
	input->reset();
	refreshInfo();
}


//refreshes info text on left side
void Graphem::refreshInfo()
{
	if(!input->auth()->ready()) {
		info_dock->setWindowTitle(tr("Welcome"));
		info_text->setText(tr("To start using Graphem, you have to set a new key pattern. Please click the \"New Pattern\" button.<br />You can find a tutorial at <a href='http://graphem.berlios.de/'>http://graphem.berlios.de/</a>"));
		input->showMessage("");
		input->setEnabled(false);
	} else {
		int usage_total = input->auth()->usageTotal();
		int usage_failed = input->auth()->usageFailed();

		info_dock->setWindowTitle(tr("Statistics"));
		info_text->setText(tr("Total: %1 <br />\
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


void Graphem::showAboutDialog()
{
	QMessageBox::about(this, tr("About Graphem"),
	tr("<center><h1>%1</h1>\
<h3>A small mouse gesture based authentication program and screen locker</h3>\
<p>Documentation is available on <a href=\"http://graphem.berlios.de/\">http://graphem.berlios.de</a></p>\
<small><p>&copy;2009 Christian Pulvermacher &lt;pulvermacher@gmx.de&gt;</p></small></center>\
<p>%2</p></small>")
	.arg(graphem_version)
	.arg("This program is free software; you can redistribute it and/or modify<br> it under the terms of the GNU General Public License as published by<br> the Free Software Foundation; either version 2 of the License, or<br> (at your option) any later version."));
}


void Graphem::showGeneratePatternDialog()
{
	GeneratePattern *dialog = new GeneratePattern(this);
	if(dialog->exec() == QDialog::Accepted) {
		input->resetAuth();
		refreshInfo();
	}

	delete dialog;
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
	if(new_pattern_dialog->exec() == QDialog::Accepted) {
		input->resetAuth();
		refreshInfo();
	}

	delete new_pattern_dialog;
}


void Graphem::quit()
{
	input->quit();
	close();
}


void Graphem::showPreferences()
{
	Preferences *pref = new Preferences(this);
	pref->exec();

	input->resetAuth();
	input->reset();

	delete pref;
}
