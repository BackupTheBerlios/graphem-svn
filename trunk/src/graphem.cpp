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
#include "auth.h"
#include "crypto.h"
#include "mainwindow.h"
#include "inputwidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QShortcut>
#include <QString>
#include <QtCrypto>

#include <iostream>

void printHelp(char *arg0);
enum WindowMode { CONFIG, ASK, LOCK };


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Graphem");
	app.setApplicationName("Graphem");

	QCA::Initializer crypto_init;
	InputWidget *input = new InputWidget();

	WindowMode mode = CONFIG;
	int tries = 0; //ignored if mode != ASK

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			printHelp(argv[0]);
			return 0;
		} else if(argv[i] == QString("--ask")) {
			mode = ASK;
		} else if(argv[i] == QString("--lock")) {
			mode = LOCK;
		} else if(argv[i] == QString("--tries")) {
			if(i+1 >= argc)
				break; //parameter not found

			tries = QString(argv[i+1]).toInt();
			i++;
		} else if(argv[i] == QString("-v") or argv[i] == QString("--verbose")) {
			input->auth()->setVerbose(true);

		}

#ifndef NO_DEBUG
		else if(argv[i] == QString("--print-data")) {
			QObject::connect(input, SIGNAL(dataReady()),
				input, SLOT(printData()));
		} else if(argv[i] == QString("--print-pattern")) {
			input->auth()->setPrintPattern(true);
		}
#endif

		else {
			std::cerr << "Unknown command line option '" << argv[i] << "'\n";
			printHelp(argv[0]);
			return 1;
		}
	}

	if(mode == CONFIG) { //show main window
		MainWindow *main = new MainWindow(input);
		//main->setWindowIcon(QIcon("icon.png"));
		main->setWindowTitle(GRAPHEM_VERSION);
		main->show();
	} else {
		if(!input->hashLoaded()) {
			std::cerr << "Couldn't load key pattern! Please start Graphem without any arguments to create one.\n";
			return 1;
		}

		QObject::connect(input->auth(), SIGNAL(passed()),
			input, SLOT(quit()));

		//input->setWindowIcon(QIcon("icon.png"));

		if(mode == ASK) {
			input->setWindowTitle(QObject::tr("%1 - Press ESC to cancel").arg(GRAPHEM_VERSION));
			new QShortcut(QKeySequence("Esc"), input, SLOT(exit()));
			input->auth()->setTries(tries);
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

	return app.exec();
}


void printHelp(char *arg0)
{
	std::cout << "Usage: " << arg0 << " [options]\n\n"
	<< "--ask\t\t Ask for key pattern but don't give access to configuration; can be canceled\n"
	<< "--help\t\t Show this text\n"
	<< "--lock\t\t Lock screen (Make sure your key pattern works!)\n"
	<< "--tries [n]\t Abort after [n] tries; can only be used with --ask\n"
	<< "-v, --verbose\t Print success/failure messages on standard output\n"
	<< "\n Returns 0 on success, 1 if canceled or maximum number of tries reached\n";

#ifndef NO_DEBUG
	std::cout << "Debug options:\n"
	<< "--print-data\t Prints velocity/pressure data to standard output\n"
	<< "--print-pattern\t Prints entered pattern as a string\n";
#endif
}
