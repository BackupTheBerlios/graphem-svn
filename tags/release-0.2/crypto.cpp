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

#include "crypto.h"

#include <cstdlib>
#include <ctime>

//TODO: not in any way secure, but does add some complexity to brute forcing
QByteArray generateSalt()
{
	const int salt_bytes_count = 5;
	srand(time(0));
	QByteArray salt;

	for(int i=0; i < salt_bytes_count; i++) {
			char c = int(double(rand())/RAND_MAX*255.0)-127;
			salt.append(c);
	}
	return salt;
}
