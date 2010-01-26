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

#include "debug_auth.h"
#include "inputwidget.h"

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QPainter>
#include <QPen>
#include <QPluginLoader>
#include <QtPlugin>
#include <QTime>

#include <iostream>

Q_EXPORT_PLUGIN2(debug, Debug)


Debug::Debug()
{
	QDir plugins_dir = QDir(qApp->applicationDirPath());
	plugins_dir.cd("plugins");

	QString plugin_file = plugins_dir.absoluteFilePath("libfcc.so");

	QPluginLoader loader(plugin_file);
	auth = qobject_cast<Auth* >(loader.instance());
	if(!auth)
		std::cout << "loading " << qPrintable(plugin_file) << "failed\n";

	if(auth)
		connect(auth, SIGNAL(checkResult(bool)), this, SIGNAL(checkResult(bool)));
}

Debug::~Debug() { delete auth; }

//print  pen_up(pressure) and velocity over time
void Debug::check()
{
	if(input->path.empty())
		return;
	std::cout << "t\tp(t)\tv(t)\n";
	QTime start = input->path.at(0).time;
	for(int i = 1; i< input->path.count(); i++) {
		double time = start.msecsTo(input->path.at(i).time);
		double length = QLineF(input->path.at(i-1).pos, input->path.at(i).pos).length();
		double v = length / input->path.at(i-1).time.msecsTo(input->path.at(i).time);
		double pressure = (input->path.at(i-1).pressure + input->path.at(i).pressure)/2;

		std::cout << time << "\t";
		std::cout << pressure << "\t";
		std::cout << v << "\n";
	}

	auth->check();
}

bool Debug::hashLoaded()
{
	if(auth)
		return auth->hashLoaded();
	else
		return false;
}

NewGesture* Debug::newGesture() { return auth->newGesture(); }

void Debug::setInput(InputWidget *i)
{
	Auth::setInput(i);
	auth->setInput(i);
}

void Debug::draw(QPainter* painter)
{
	auth->draw(painter);

	if(input->path.empty())
		return;

	QPen pen;
	for(int i = 1; i< input->path.count(); i++) {
		double length = QLineF(input->path.at(i-1).pos, input->path.at(i).pos).length();
		double v = length / input->path.at(i-1).time.msecsTo(input->path.at(i).time);
		double pressure = (input->path.at(i-1).pressure + input->path.at(i).pressure)/2;
		if(v != v)
			continue;

		pen.setColor(QColor(255 - v*13, 13*v, 0));
		pen.setWidth(10*pressure);
		painter->setPen(pen);
		painter->drawLine(input->path.at(i-1).pos, input->path.at(i).pos);
	}

}

void Debug::reset() { auth->reset(); }
