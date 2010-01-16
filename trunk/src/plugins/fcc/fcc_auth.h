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

#ifndef FCC_AUTH_H
#define FCC_AUTH_H

#include "auth.h"
#include "node.h"
#include "fcc_stroke.h"

#include <QByteArray>
#include <QLineF>
#include <QList>
#include <QObject>
#include <QString>

class NewGesture;
class QTime;

class FCC : public Auth {
	Q_OBJECT
	Q_INTERFACES(Auth)
	friend class FCCNewGesture;
public:
	FCC();

	bool hashLoaded() { return hash_loaded; }
	NewGesture* newGesture();
	void preprocess();
	virtual void setInput(InputWidget *i);
public slots:
	void check();
	void reset();
signals:
	void checkResult(bool correct);
protected:
	void loadHash();
	bool matchesAuthGesture();
	QString strokesToString();
	bool tryGesture();

	QByteArray auth_gesture, salt;
	QList<Stroke> strokes;
	int compared_hashes_count;
	QTime *started;
	bool hash_loaded;
	bool touchpad_mode;
	int check_timeout; //in ms

	const static int short_limit = 10; //length limit for short strokes
};
#endif
