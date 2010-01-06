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
#include "graphem.h"
#include "inputwidget.h"
#include "mainwindow.h"
#include "newgesture.h"
#include "preferences.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QTextEdit>


MainWindow::MainWindow(InputWidget* input):
	input(input),
	new_gesture_dialog(0),
	info_text(0)
{
	setCentralWidget(input);

	//menu bar
	QMenu *file = menuBar()->addMenu(tr("&File"));
	file->addAction(tr("&New Gesture..."), this,
		SLOT(showNewGestureDialog()), tr("Ctrl+N"));
	edit_action = file->addAction(tr("&Edit Gesture..."), this,
		SLOT(showEditGestureDialog()), tr("Ctrl+E"));

	file->addSeparator();
	save_action = file->addAction(tr("&Save"), this,
		SLOT(save()), tr("Ctrl+S"));

	file->addSeparator();
	file->addAction(tr("&Preferences"), this, SLOT(showPreferences()));

	file->addSeparator();
	file->addAction(tr("&Quit"), this, SLOT(close()), tr("Ctrl+Q"));

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
	setUnsavedChanges(false);

	connect(Graphem::getAuth(), SIGNAL(checkResult(bool)),
		this, SLOT(reset()));

	refreshInfo();
}


void MainWindow::closeEvent(QCloseEvent* ev)
{
	if(unsaved_changes) {
		QMessageBox::StandardButton button = QMessageBox::warning(this, "",
			"<b>Save the new key gesture?</b>",
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
			QMessageBox::Save);
		if(button == QMessageBox::Cancel) {
			ev->ignore();
			return;
		} else if(button == QMessageBox::Save) {
			save();
		}
	}
	ev->accept();
}


//refreshes info text on left side
void MainWindow::refreshInfo()
{
	if(!Graphem::getAuth()->hashLoaded()) {
		info_dock->setWindowTitle(tr("Welcome"));
		info_text->setText(tr("To start using Graphem, you have to set a new key gesture (File -> New Gesture).<br />You can find a tutorial at <a href='http://graphem.berlios.de/'>http://graphem.berlios.de/</a>"));
		input->showMessage("");
		input->setEnabled(false);
	} else {
		QSettings settings;
		int usage_total = settings.value("usage_total").toInt();
		int usage_failed = settings.value("usage_failed").toInt();

		info_dock->setWindowTitle(tr("Statistics"));
		info_text->setText(tr("Total: %1 <br />\
			Correct: %2 / Failed: %3 <br />\
			Recognition Rate: %4\%")
			.arg(usage_total)
			.arg(usage_total-usage_failed)
			.arg(usage_failed)
			.arg(qRound(double(usage_total - usage_failed)/usage_total*1000)/10.0));
		input->setEnabled(true);
		input->setDefaultMessage(tr("You can test your gesture here."));
		input->showMessage();
	}
}


void MainWindow::reset()
{
	input->reset();
	refreshInfo();
}


//save gesture currently being tested
void MainWindow::save()
{
	Q_ASSERT(new_gesture_dialog != 0);
	new_gesture_dialog->save();
	setUnsavedChanges(false);
}


void MainWindow::setUnsavedChanges(bool b)
{
	unsaved_changes = b;
	edit_action->setEnabled(b);
	save_action->setEnabled(b);
}


void MainWindow::showAboutDialog()
{
	QMessageBox::about(this, tr("About Graphem"),
	tr("<center><h1>%1</h1>\
<h3>A small mouse gesture based authentication program and screen locker</h3>\
<p>Documentation is available on <a href=\"http://graphem.berlios.de/\">http://graphem.berlios.de</a></p>\
<small><p>&copy;2009 Christian Pulvermacher &lt;pulvermacher@gmx.de&gt;</p></small></center>\
<p>%2</p></small>")
	.arg(GRAPHEM_VERSION)
	.arg("This program is free software; you can redistribute it and/or modify<br> it under the terms of the GNU General Public License as published by<br> the Free Software Foundation; either version 2 of the License, or<br> (at your option) any later version."));
}


//show the NewGesture dialog again without resetting it
void MainWindow::showEditGestureDialog()
{
	if(new_gesture_dialog->exec() == QDialog::Accepted) {
		Graphem::getAuth()->reset();
		refreshInfo();

		setUnsavedChanges(true);
	}
}


//create new NewGesture dialog and show it
void MainWindow::showNewGestureDialog()
{
	if(new_gesture_dialog)
		delete new_gesture_dialog;
	new_gesture_dialog = Graphem::getAuth()->newGesture();

	showEditGestureDialog();
}


void MainWindow::showPreferences()
{
	Preferences *pref = new Preferences(this);
	pref->exec();

	input->reset();

	delete pref;
}
