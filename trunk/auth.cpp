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

#include <QLineF>
#include <QString>
#include <QtDebug>

#include "auth.h"
#include "inputwidget.h"

QString Auth::strokesToString(QList<Stroke> l)
{
	QString result = "";
	for(int i = 0; i < l.count(); i++) {
		if(l.at(i).removed)
			continue;
		result += QString::number(l.at(i).direction);
		result += (l.at(i).up ? '#': '_');
	}
	return result;
}

Auth::Auth(QObject *parent):
	QObject(parent),
	auth_pattern(""),
	started(0)
{ }

void Auth::setAuthPattern(QString pattern) { auth_pattern = pattern; } 

void Auth::preprocess(const QList<Node> &path)
{
	strokes.clear();
	indices.clear();
	int start = 0; //start node
	bool pen_down = true;

	for(int i = 0; i < path.count(); i++) {
		QPointF a = path.at(start).pos;
		if(pen_down and i > 0 and path.at(i).isSeparator()) { //pen up, end stroke here
			QLineF l = QLineF(path.at(start).pos, path.at(i-1).pos);
			strokes.append(Stroke(l));

			pen_down = false;
			start = i-1;
		} else if(!pen_down and i > 1) { //add virtual stroke
			QLineF l = QLineF(path.at(start).pos, path.at(i).pos);
			strokes.append(Stroke(l, true));

			pen_down = true;
			start = i;
		} else if(pen_down and !path.at(i).isSeparator()){
			QLineF l = QLineF(path.at(start).pos, path.at(i).pos);
			if(l.length() > short_limit) {
				strokes.append(Stroke(l));
				start = i;
			}
		}
	}
	combineStrokes(strokes);
	//actually delete the marked strokes
	for(int i = 0; i < strokes.count(); ) {
		if(strokes.at(i).removed)
			strokes.removeAt(i);
		else
			i++;
	}
}

void Auth::combineStrokes(QList<Stroke> &s)
{
	int lastdirection = -1; //invalid direction
	for(int i = 0; i < s.count(); i++) {
		if(s.at(i).removed)
			continue; //skip removed strokes

		if(s.at(i).up)
			lastdirection = -1;
		else if(s.at(i).direction == lastdirection) {
			s[i-1] += s.at(i);
			s[i].removed = true;
			i--;
		} else
			lastdirection = s.at(i).direction;
	}
}

bool Auth::tryPattern()
{
	if(matchesAuthPattern(strokes))
		return true;
	
	int strokes_count = strokes.count();
	int offset[strokes_count]; //stores current permutation
	for(int i=0; i < strokes_count; i++)
		offset[i] = 0;

	for(int n = 0; started->elapsed() < max_check_time; n++) {
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
		combineStrokes(strokes);

		if(matchesAuthPattern(strokes))
			return true;
	}
	return false;
}


/*
//tries variations of s, changes occur at indices[i]
bool Auth::tryPattern(QList<Stroke> s, int i)
{
	//TODO: maybe change direction by more than 1
	if(i == indices.count()) //recursion went through all strokes
		return matchesAuthPattern(s);

	if(started->elapsed() > max_check_time)
		return false;

	//TODO: could be improved by only checking the vicinity of modified strokes (indices[i-1] +- 1)
	combineStrokes(s);
	
	int index = indices.at(i);
	//try unchanged
	if(tryPattern(s, i+1))
		return true;

	//try removing stroke
	if(!s.at(index).up) {
		s[index].removed = true;
		if(tryPattern(s, i+1))
			return true;
		s[index].removed = false;
	} 

	//try decreasing direction
	if(s.at(index).direction == 0)
		s[index].direction = 7;
	else
		s[index].direction--;
	if(tryPattern(s, i+1))
		return true;
	
	//try increasing direction
	s[index].direction = (s.at(index).direction + 2) % 8;

	return tryPattern(s, i+1);
}
*/

//TODO replace this with a cryptographic hash + salt
bool Auth::matchesAuthPattern(const QList<Stroke> &s)
{
#ifndef NO_DEBUG
	tries++;
#endif

	return auth_pattern == strokesToString(s);
}

void Auth::check()
{
	if(auth_pattern.isEmpty()) {
		emit passed();
		return;
	}

	qDebug() << "number of strokes: " << strokes.count();

	tries = 0;
	started = new QTime();
	started->start();

	//sort stroke indices by weight (ascending)
	for(int i = 0; i < strokes.count(); i++) {
		int lowest = -1;
		for(int j = 0; j < strokes.count(); j++) {
			if((lowest == -1 or strokes.at(j).getWeight() < strokes.at(lowest).getWeight())
			and !indices.contains(j))
				lowest = j;
		}
		indices.append(lowest);
	}

	if(tryPattern()) {
		emit passed();
	} else {
		if(started->elapsed() > max_check_time) {
			qDebug() << "Processing timed out, could not match pattern";
		}
		emit failed();
	}

	double time = double(started->elapsed())/1000;
	qDebug() << tries << " tries in " << time << "s, or " << tries/time << "tries/s";
	delete started;
}


void Auth::printPattern()
{
	std::cout << qPrintable(strokesToString(strokes)) << "\n";
}
