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

#ifndef AUTH_H
#define AUTH_H

#include "newpattern.h"
#include "node.h"
#include "stroke.h"

#include <QByteArray>
#include <QLineF>
#include <QList>
#include <QObject>
#include <QString>

class QTime;

class Auth : public QObject { //TODO needs cleanup
	Q_OBJECT
	friend class NewPattern;
	friend class GeneratePattern;
	friend class InputWidget;
public:
	Auth(QObject *parent = 0);
	void loadHash();
	void preprocess(const QList<Node> &path);
	void saveHash();
	void saveStats();
	void setTries(int tries) { tries_left = tries; }
	void setVerbose(bool on) { verbose = on; }
	void setPrintPattern(bool on) { print_pattern = on; }
	int usageFailed() { return usage_failed; }
	int usageTotal() { return usage_total; }
	bool usingTouchpadMode() { return touchpad_mode; }
public slots:
	void check();
signals:
	void failed();
	void passed();
private:
	bool matchesAuthPattern();
	void setAuthHash(const QByteArray &hash, const QByteArray &s) { auth_pattern = hash; salt = s; }
	QString strokesToString();
	bool tryPattern();

	QByteArray auth_pattern, salt;
	QList<Stroke> strokes;
	int compared_hashes_count;
	int tries_left;
	QTime *started;
	bool hash_loaded;
	bool print_pattern;
	bool touchpad_mode;
	bool verbose;
	int check_timeout; //in ms
	int usage_total, usage_failed; //usage statistics for pattern

	const static int short_limit = 10; //length limit for short strokes
};
#endif
