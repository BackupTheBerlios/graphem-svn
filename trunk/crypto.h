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

#ifndef CRYPTO_H
#define CRYPTO_H

#include <QCryptographicHash>
#include <QString>

namespace Crypto {
	QByteArray generateSalt();
	QByteArray getHash(const QString &s, const QByteArray &salt);
	int randInt(int min, int max);
};


inline
QByteArray Crypto::getHash(const QString &s, const QByteArray &salt)
{
	return QCryptographicHash::hash(s.toAscii() + salt, QCryptographicHash::Sha1);
}
#endif
