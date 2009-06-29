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

#include <QString>

#include "auth.h"
#include "inputwidget.h"
#include "main.h"

Graphem::Graphem(int argc, char* argv[]) :
	QApplication(argc, argv),
	input(new InputWidget()),
	auth(new Auth(this)),
	tries_left(0),
	print_pattern(false),
	status(-1)
{
	connect(input, SIGNAL(finished()),
		this, SLOT(authenticate()),
		Qt::QueuedConnection); //allow for repaint before checking

	connect(auth, SIGNAL(failed()),
		this, SLOT(failed()));
	connect(auth, SIGNAL(passed()),
		this, SLOT(quit()));

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			std::cout << "Usage: " << argv[0] << " [options]\n\n";
			std::cout << "--help\t\t\t Show this text\n";
			std::cout << "--lock\t\t\t Lock screen (Make sure your auth pattern works!)\n";
			std::cout << "--pattern [pattern]\t Specify a recorded pattern to check against\n";
			std::cout << "--print-data\t\t Prints velocity/pressure data to standard output\n";
			std::cout << "--print-pattern\t\t Prints entered pattern as a string\n";
			std::cout << "--show-input\t\t Shows input while drawing\n";
			std::cout << "--touchpad\t\t Touchpad mode, no clicking necessary\n";
			std::cout << "--tries [n]\t\t Exit Graphem with status code 1 after [n] tries; 0 to disable (default)\n";
			status = 0;
			return;
		} else if(argv[i] == QString("--lock")) {
			input->enableLocking(true);
		} else if(argv[i] == QString("--pattern")) {
			if(i+1 >= argc)
				break; //parameter not found

			auth->setAuthPattern(argv[i+1]);
			i++;
		} else if(argv[i] == QString("--print-data")) {
			connect(input, SIGNAL(finished()),
				input, SLOT(printData()));
		} else if(argv[i] == QString("--print")) {
			print_pattern = true;
		} else if(argv[i] == QString("--show-input")) {
			input->showInput(true);
		} else if(argv[i] == QString("--touchpad")) {
			input->enableTouchpadMode(true);
		} else if(argv[i] == QString("--tries")) {
			if(i+1 >= argc)
				break; //parameter not found

			tries_left = QString(argv[i+1]).toInt();
			i++;
		}
	}

	input->setWindowTitle("graphem 0.1");
	input->show();

	status = exec();
}

void Graphem::failed()
{
	if(tries_left == 1) //this try was our last
		qApp->exit(1);

	if(tries_left != 0) //0 is for infinite amount of tries, don't decrease
		tries_left--;
	input->reset();
}

void Graphem::authenticate()
{
	auth->preprocess(input->path);
	if(print_pattern)
		auth->printPattern();
	auth->check();
}


int main(int argc, char* argv[])
{
	Graphem graphem(argc, argv);
	return graphem.getStatus();
}
