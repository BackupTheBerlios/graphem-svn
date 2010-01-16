/*
    Graphem
    Copyright (C) 2009-2010 Christian Pulvermacher

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

#include <QObject>
#include <QString>

const QString GRAPHEM_VERSION = "Graphem 0.4";
enum WindowMode { CONFIG, ASK, LOCK };

//default values for various settings
const int CHECK_TIMEOUT = 4; //in s
const bool FADE = true;
const bool SHOW_INPUT = false;
const double WINDOW_OPACITY = 1.0;
const int FADE_TIME = 1500; //in ms
const int FADE_STEP_TIME = 50; //in ms



class Auth;
class InputWidget;

class Graphem : public QObject {
	Q_OBJECT
public:
	Graphem(WindowMode mode, Auth *a);
	~Graphem();
	void cleanup();
	void setMaxTries(int t) { max_tries = t; }
	void setVerbose(bool on) { verbose = on; }

	static Auth* getAuth(); //returns the main auth instance
	static Auth* loadAuthPlugin(QString name);
public slots:
	void abort(); //exit with status code 1
	void checkResult(bool correct);
	void quit(); //exit with status code 0
private:
	WindowMode mode;
	InputWidget *input;
	static Auth *auth;
	int max_tries;
	int tries;
	bool verbose;
};
#endif
