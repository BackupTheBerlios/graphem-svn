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
	double length = 0;
	for(int i = 0; i< input->path.count(); i++) {
		if(i > 0)
			length = QLineF(input->path.at(i-1).pos, input->path.at(i).pos).length();
		double time = start.msecsTo(input->path.at(i).time);
		std::cout << time << "\t";
		std::cout << int(input->path.at(i).pressure) << "\t";
		std::cout << length/time << "\n";
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
	pen.setColor(Qt::yellow);
	QTime start = input->path.at(0).time;
	double length = 0;
	for(int i = 0; i< input->path.count(); i++) {
		if(i > 0)
			length = QLineF(input->path.at(i-1).pos, input->path.at(i).pos).length();
		double v = length / start.msecsTo(input->path.at(i).time);
		if(v != v or v > 0.1)
			continue;
		pen.setWidth(500*v);
		painter->setPen(pen);
		painter->drawLine(input->path.at(i-1).pos, input->path.at(i).pos);
	}

}

void Debug::reset() { auth->reset(); }
