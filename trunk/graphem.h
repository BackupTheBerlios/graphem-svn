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

#include <QMainWindow>

class Auth;
class InputWidget;
class QSettings;
class QTextEdit;

const QString graphem_version = "Graphem 0.3";

class Graphem : public QMainWindow {
	Q_OBJECT
public:
	Graphem(Auth *auth);
public slots:
	void authenticate();
	void failed();
	void passed();
	void refreshInfo();
	void showNewPatternDialog();
	void quit();
private:
	void resetStats();

	InputWidget* input;
	Auth* auth;
	QSettings* settings;
	QTextEdit* info_text;

	int usage_total, usage_failed; //usage statistics for pattern
};
#endif
