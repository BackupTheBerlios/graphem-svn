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
#include "crypto.h"

#ifndef NO_QCA
#include <QtCrypto>
#endif

#include <QApplication>

#include <cstdlib>
#include <iostream>

void printHelp(char *arg0);


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Graphem");
	app.setApplicationName("Graphem");

#ifndef NO_QCA
	QCA::Initializer crypto_init;
#else
	qsrand(time(0));
#endif

	WindowMode mode = CONFIG;
	int tries = 0; //ignored if mode != ASK
	QString plugin("fcc");
	bool verbose = false;

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			printHelp(argv[0]);
			return 0;
		} else if(argv[i] == QString("--ask")) {
			mode = ASK;
		} else if(argv[i] == QString("--lock")) {
			mode = LOCK;
		} else if(argv[i] == QString("--plugin")) {
			if(i+1 >= argc)
				break; //parameter not found

			plugin = QString(argv[i+1]);
			i++;
		} else if(argv[i] == QString("--tries")) {
			if(i+1 >= argc)
				break; //parameter not found

			tries = QString(argv[i+1]).toInt();
			i++;
		} else if(argv[i] == QString("-v") or argv[i] == QString("--verbose")) {
			verbose = true;
		} else {
			std::cerr << "Unknown command line option '" << argv[i] << "'\n";
			printHelp(argv[0]);
			return 1;
		}
	}

	Auth *auth = Graphem::loadAuthPlugin(plugin);
	if(!auth) {
		if(mode != CONFIG) {
			std::cerr << "Couldn't load plugin! TODO: Add helpful suggestions\n";
			return 1;
		} else {
			std::cerr << "Couldn't load plugin, but we're in CONFIG mode ... TODO\n";
		}
	}

	Graphem graphem(mode, auth);
	graphem.setMaxTries(tries);
	graphem.setVerbose(verbose);

	if(mode != CONFIG and !graphem.getAuth()->hashLoaded()) {
		std::cerr << "Couldn't load key gesture! Please start Graphem without any arguments to create one.\n";
		return 1;
	}

	return app.exec();
}


void printHelp(char *arg0)
{
	std::cout << "Usage: " << arg0 << " [options]\n\n"
	<< "--ask\t\t Ask for key gesture but don't give access to configuration; can be canceled\n"
	<< "--help\t\t Show this text\n"
	<< "--lock\t\t Lock screen (Make sure your key gesture works!)\n"
	<< "--plugin [name]\t Load the auth plugin [name] instead of the configured one\n"
	<< "--tries [n]\t Abort after [n] tries; can only be used with --ask\n"
	<< "-v, --verbose\t Print success/failure messages on standard output\n"
	<< "\n Returns 0 on success, 1 if canceled or maximum number of tries reached\n";
}
