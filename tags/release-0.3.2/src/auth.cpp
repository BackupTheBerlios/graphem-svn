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
#include "crypto.h"

#include <cmath>
#include <iostream>

#include <QLineF>
#include <QSettings>
#include <QString>
#include <QtDebug>


Auth::Auth(QObject *parent):
	QObject(parent),
	tries_left(0),
	started(0),
	hash_loaded(false),
	print_pattern(false),
	testing_pattern(false),
	touchpad_mode(false),
	verbose(false),
	check_timeout(6000)
{ }


void Auth::check()
{
	qDebug() << "number of strokes: " << strokes.count();

	if(auth_pattern.isEmpty()) {
		emit passed();
		return;
	}

	compared_hashes_count = 0;
	started = new QTime();
	started->start();

	usage_total++;
	if(tryPattern()) {
		if(verbose)
			std::cout << "OK: Correct pattern.\n";

		emit passed();
	} else {
		usage_failed++;
		if(verbose)
			std::cout << "ERROR: Pattern not recognized.\n";


		if(tries_left == 1) {//this try was our last
			saveStats();
			exit(1);
		}

		if(tries_left != 0) //0 is for infinite amount of tries, don't decrease
			tries_left--;

		emit failed();
	}

#ifndef NO_DEBUG
	double time = double(started->elapsed())/1000;
	qDebug() << compared_hashes_count << " tries in " << time << "s, or " << compared_hashes_count/time << "tries/s";

	if(print_pattern)
		qDebug() << "Input: " << strokesToString();
#endif

	delete started;
}


//set up Auth module with hash from config file
void Auth::loadHash()
{
	QSettings settings;
	if(settings.value("pattern_hash").toByteArray().isEmpty()
	or settings.value("salt").toByteArray().isEmpty()) {
		hash_loaded = false;
		return;
	}

	setAuthHash(settings.value("pattern_hash").toByteArray(), settings.value("salt").toByteArray());

	touchpad_mode = false;
	if(settings.value("touchpad_mode").toBool())
		touchpad_mode = true;

	usage_total = settings.value("usage_total").toInt();
	usage_failed = settings.value("usage_failed").toInt();
	check_timeout = settings.value("check_timeout", 6).toInt() * 1000;

	hash_loaded = true;
	testing_pattern = false;
}


bool Auth::matchesAuthPattern()
{
#ifndef NO_DEBUG
	compared_hashes_count++;
#endif

	return auth_pattern == Crypto::getHash(strokesToString(), salt);
}


//analyses path and stores the result in strokes
void Auth::preprocess(const QList<Node> &path)
{
	strokes.clear();
	int start = 0; //start node
	bool pen_down = true;

	for(int i = 0; i < path.count(); i++) {
		QPointF a = path.at(start).pos;
		if(pen_down and i > 0 and path.at(i).pen_up) { //pen up, end stroke here
			QLineF l = QLineF(a, path.at(i).pos);
			strokes.append(Stroke(l, start));

			pen_down = false;
			start = i;
		} else if(!pen_down and i > 1) { //add virtual stroke
			QLineF l = QLineF(a, path.at(i).pos);
			strokes.append(Stroke(l, start, true));

			pen_down = true;
			start = i;
		} else if(pen_down and !path.at(i).pen_up){
			QLineF l = QLineF(a, path.at(i).pos);
			if(l.length() > short_limit) {
				strokes.append(Stroke(l, start));
				start = i;
			}
		}
	}

	//remove duplicate strokes
	int lastdirection = -1; //invalid direction
	for(int i = 0; i < strokes.count(); i++) {
		if(strokes.at(i).up)
			lastdirection = -1;
		else if(strokes.at(i).direction == lastdirection) {
			strokes[i-1] += strokes.at(i);
			strokes.removeAt(i);
			i--;
		} else
			lastdirection = strokes.at(i).direction;
	}
}


void Auth::saveStats()
{
	if(hash_loaded and !testing_pattern) {
		QSettings settings;
		settings.setValue("usage_total", usage_total);
		settings.setValue("usage_failed", usage_failed);
		settings.sync();
	}
}


//converts strokes into a string, omits 'removed' and duplicate strokes
QString Auth::strokesToString()
{
	QString result = "";
	int lastdir = -1;
	for(int i = 0; i < strokes.count(); i++) {
		if(strokes.at(i).removed)
			continue;
		if(!strokes.at(i).up) {
			if(strokes.at(i).direction == lastdir)	
				continue;
			lastdir = strokes.at(i).direction;
		}
		result += QString::number(strokes.at(i).direction);
		result += (strokes.at(i).up ? '#': '_');
	}
	return result;
}


bool Auth::tryPattern()
{
	//test unchanged
	if(matchesAuthPattern())
		return true;

	//sort stroke indices by weight (ascending)
	QList<int> indices;
	for(int i = 0; i < strokes.count(); i++) {
		int lowest = -1;
		for(int j = 0; j < strokes.count(); j++) {
			if((lowest == -1 or strokes.at(j).getWeight() < strokes.at(lowest).getWeight())
			and !indices.contains(j))
				lowest = j;
		}
		indices.append(lowest);
	}

	const int strokes_count = strokes.count();
	const int permutations_count = pow(4, strokes_count);
	int offset[strokes_count]; //stores current permutation
	for(int i=0; i < strokes_count; i++)
		offset[i] = 0;

	for(int n = 1; n != permutations_count; n++) {
		if(started->elapsed() > check_timeout)
			break;

		int p = 1;
		for(int i = 0; i < strokes_count; i++) {
			if((n % p) != 0) // all changes for this n done
				break;

			offset[i] = (offset[i]+1)%4;

			int index = indices.at(i);
			switch(offset[i]) {
			case 0: // unchanged
				break;
			case 1: // remove
				if(!strokes.at(index).up)
					strokes[index].removed = true;
				break;
			case 2: // direction-1
				strokes[index].removed = false;
				if(strokes.at(index).direction == 0)
					strokes[index].direction = 7;
				else
					strokes[index].direction--;
				break;
			case 3: // direction+1
				strokes[index].direction += 2;
				strokes[index].direction %= 8;
				break;
			}
			p *= 4; // period increases by this amount as we move up the tree
		}

		if(matchesAuthPattern())
			return true;
	}
	return false;
}
