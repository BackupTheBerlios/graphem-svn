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

#ifndef NODE_H
#define NODE_H

#include <QPointF>
#include <QTime>

struct Node {
public:
	Node(QPointF pos, QTime time, qreal pressure = 1, bool s = false) :
		pos(pos),
		time(time),
		separator(s),
		pressure(pressure)
	{ }
	static Node makeSeparator() {
		return Node(QPointF(), QTime(QTime::currentTime()), 0, true);
	}
	bool isSeparator() const { return separator; }

	QPointF pos;
	QTime time;
	bool separator;
	qreal pressure;
};
#endif
