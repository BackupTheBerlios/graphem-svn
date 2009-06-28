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

#include <iostream>

#include <QApplication>
#include <QString>

#include "auth.h"
#include "inputwidget.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	InputWidget *input = new InputWidget;
	Auth *auth = new Auth(input);

	QObject::connect(input, SIGNAL(finished()),
		auth, SLOT(check()),
		Qt::QueuedConnection); //allow for repaint before checking
	QObject::connect(auth, SIGNAL(failed()),
		input, SLOT(reset()));
	QObject::connect(auth, SIGNAL(passed()),
		qApp, SLOT(quit()));

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			std::cout << "Usage: " << argv[0] << " [options]\n\n";
			std::cout << "--help\t\t\t Show this text\n";
			std::cout << "--lock\t\t\t Lock screen (Make sure your auth pattern works!)\n";
			std::cout << "--pattern [pattern]\t Specify a recorded pattern to check against\n";
			std::cout << "--print\t\t\t Prints entered pattern as a string\n";
			std::cout << "--print-data\t\t Prints velocity/pressure data to standard output\n";
			std::cout << "--show-input\t\t Shows input while drawing\n";
			std::cout << "--touchpad\t\t Touchpad mode, no clicking necessary\n";
			return 0;
		} else if(argv[i] == QString("--lock")) {
			input->enableLocking(true);
		} else if(argv[i] == QString("--pattern")) {
			if(i+1 >= argc)
				break; //no pattern found

			auth->setAuthPattern(argv[i+1]);
			i++;
		} else if(argv[i] == QString("--print")) {
			QObject::connect(input, SIGNAL(finished()),
				auth, SLOT(printPattern()));
		} else if(argv[i] == QString("--print-data")) {
			QObject::connect(input, SIGNAL(finished()),
				auth, SLOT(printData()));
		} else if(argv[i] == QString("--show-input")) {
			input->showInput(true);
		} else if(argv[i] == QString("--touchpad")) {
			input->enableTouchpadMode(true);
		}
	}

	input->setWindowTitle("graphem 0.1 beta");
	input->show();

	return app.exec();
}
