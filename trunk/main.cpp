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

#include <QApplication>
#include <QDesktopWidget>
#include <QString>
#include <QtCrypto>

#include <iostream>

void printHelp(char *arg0);

using namespace std;


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Graphem");
	app.setApplicationName("Graphem");

	QCA::Initializer crypto_init;
	InputWidget *input = new InputWidget();

	bool lock_screen = false;

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			printHelp(argv[0]);
			return 0;
		} else if(argv[i] == QString("--lock")) {
				lock_screen = true;
#ifndef NO_DEBUG
		} else if(argv[i] == QString("--print-data")) {
			QObject::connect(input, SIGNAL(dataReady()),
				input, SLOT(printData()));
		} else if(argv[i] == QString("--print-pattern")) {
			input->auth()->setPrintPattern(true);
#endif
		} else if(argv[i] == QString("--tries")) {
			if(i+1 >= argc)
				break; //parameter not found

			input->auth()->setTries(QString(argv[i+1]).toInt());
			i++;
		} else if(argv[i] == QString("-v") or argv[i] == QString("--verbose")) {
			input->auth()->setVerbose(true);
		} else {
			cerr << "Unknown command line option '" << argv[i] << "'\n";
			printHelp(argv[0]);
			return 1;
		}
	}

	if(lock_screen) {
		if(!input->auth()->ready()) {
			cerr << "Couldn't load key pattern!\n";
			return 1;
		}

		QObject::connect(input->auth(), SIGNAL(passed()),
			input, SLOT(quit()));

		input->setWindowTitle(graphem_version);
		input->setWindowFlags(Qt::X11BypassWindowManagerHint);
		input->setVisible(true);
		QDesktopWidget dw;
		input->setGeometry(dw.screenGeometry());

		input->grabKeyboard();
		input->grabMouse();
		input->activateWindow(); //make sure we catch keyboard events
		input->raise();
	} else { //show main window
		Graphem *main = new Graphem(input);
		main->show();
	}

	return app.exec();
}


void printHelp(char *arg0)
{
	cout << qPrintable(QObject::tr("Usage: %1 [options]\n\n").arg(arg0)
	+"--help\t\t\t Show this text\n"
	+"--lock\t\t\t Lock screen (Make sure your key pattern works!)\n"

#ifndef NO_DEBUG
	+"--print-data\t\t Prints velocity/pressure data to standard output\n"
	+"--print-pattern\t\t Prints entered pattern as a string\n"
#endif

	+"--tries [n]\t\t Exit Graphem with status code 1 after [n] tries; 0 to disable (default)\n"
	+"-v, --verbose\t\t Print success/failure messages on stdout\n");
}
