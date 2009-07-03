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

#ifndef MAIN_H
#define MAIN_H

#include <QApplication>

class InputWidget;
class Auth;

class Graphem : public QApplication {
	Q_OBJECT
public:
	Graphem(int argc, char* argv[]);
	int getStatus() { return status; }
	void printHelp();
public slots:
	void authenticate();
	void failed();
	void passed();
private:
	InputWidget* input;
	Auth* auth;
	int tries_left;
	bool print_pattern, verbose;
	int status;
};
#endif
