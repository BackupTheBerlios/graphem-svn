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

#include "fcc_stroke.h"

#include <QLineF>

#include <cmath>

Stroke::Stroke(QLineF l, int start_node, bool up):
	start_node_id(start_node),
	up(up),
	removed(false),
	length(l.length())
{
	//actuallly this is a real valued version of direction
	double angle = atan2(-l.dy(), l.dx()) * 4 / 3.141;
	if(angle < 0)
		angle += 8;
	Q_ASSERT(angle >= 0);

	direction = qRound(angle) % 8;

	weight = (1- (angle - qRound(angle))) * w_angle;
	weight += length * w_length;
}

//add another stroke
Stroke Stroke::operator+=(const Stroke &s)
{
	Q_ASSERT(direction == s.direction);

	length += s.length;
	weight += s.weight;

	return *this;
}
