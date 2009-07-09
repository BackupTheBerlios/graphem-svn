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

#ifndef GRAPHEM_H
#define GRAPHEM_H

#include <QApplication>

class Auth;
class InputWidget;
class NewPattern;
class QSettings;
class QTextEdit;

class Graphem : public QApplication {
	Q_OBJECT
public:
	Graphem(int argc, char* argv[]);
	int exec();
public slots:
	void authenticate();
	void failed();
	void passed();
	void refreshInfo();
	void quit();
private:
	void printHelp();

	InputWidget* input;
	Auth* auth;
	QSettings* settings;
	QTextEdit* info_text;
	NewPattern *new_pattern_dialog;

	int tries_left;
	bool print_pattern, verbose, lock_screen;
	int usage_total, usage_failed; //usage statistics for pattern
	int status;
};
#endif
