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
//#include "auth.h"
#include "fcc_auth.h"
#include "crypto.h"
#include "mainwindow.h"
#include "inputwidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QShortcut>
#include <QString>

#include <iostream>

Auth* Graphem::auth = 0;

Graphem::Graphem(WindowMode mode):
	mode(mode),
	input(new InputWidget()),
	max_tries(0),
	tries(0),
	verbose(false)
{
	auth = loadAuthPlugin();
//	std::cout << "Auth plugin '" << auth->metaObject()->className() << "' loaded.\n";;

	connect(input, SIGNAL(dataReady()),
		auth, SLOT(check()), Qt::QueuedConnection);
	connect(auth, SIGNAL(checkResult(bool)),
		this, SLOT(checkResult(bool)));
	connect(input, SIGNAL(redraw(QPainter*)),
		auth, SLOT(draw(QPainter*)), Qt::DirectConnection);

	if(mode == CONFIG) { //show main window
		MainWindow *main = new MainWindow(input);

		connect(auth, SIGNAL(checkResult(bool)),
			input, SLOT(reset()));

		//main->setWindowIcon(QIcon("icon.png"));
		main->setWindowTitle(GRAPHEM_VERSION);
		main->show();
	} else {
		if(!auth->hashLoaded()) {
			std::cerr << "Couldn't load key gesture! Please start Graphem without any arguments to create one.\n";
			abort();
		}

		//input->setWindowIcon(QIcon("icon.png"));

		if(mode == ASK) {
			input->setWindowTitle(QObject::tr("%1 - Press ESC to cancel").arg(GRAPHEM_VERSION));
			QShortcut *shortcut = new QShortcut(QKeySequence("Esc"), input);
			connect(shortcut, SIGNAL(activated()),
				this, SLOT(abort()));
			input->showMaximized();
		} else { //mode == LOCK
			input->setWindowTitle(GRAPHEM_VERSION);
			input->setGrab(true);

			//for full screen, we strip WM decorations and resize the window manually
			input->setWindowFlags(Qt::X11BypassWindowManagerHint);
			input->setVisible(true);
			QDesktopWidget dw;
			input->setGeometry(dw.screenGeometry());
		}
	}
}


Graphem::~Graphem()
{
	delete auth;
	delete input;
}


void Graphem::checkResult(bool correct)
{	
	if(verbose) {
		if(correct)
			std::cout << "OK: Correct gesture.\n";
		else
			std::cout << "ERROR: Gesture not recognized.\n";
	}

	//save statistics
	QSettings settings;
	int usage_total = settings.value("usage_total").toInt();
	int usage_failed = settings.value("usage_failed").toInt();

	usage_total++;
	if(!correct)
		usage_failed++;

	settings.setValue("usage_total", usage_total);
	settings.setValue("usage_failed", usage_failed);
	settings.sync();

	//exit program?
	if(mode != CONFIG) {
		if(correct) {
			quit();
		} else {
			tries++;
			if(mode == ASK and max_tries != 0  //max_tries is only valid with ASK
			and tries >= max_tries)
				abort();
		}

		input->reset();
	}
}

Auth* Graphem::getAuth()
{
	return auth;
}


//returns 0 if load fails
Auth* Graphem::loadAuthPlugin()
{
	return new FCC(this, input);
}


void Graphem::cleanup()
{
	input->releaseMouse();
	input->releaseKeyboard();
	input->close();
}

void Graphem::abort()
{
	cleanup();
	qApp->exit(1);
}

void Graphem::quit()
{
	cleanup();
	qApp->exit(0);
}
