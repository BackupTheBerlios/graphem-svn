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
#include "inputwidget.h"
#include "graphem.h"

#include <iostream>

#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

using namespace std;

const QString version = "Graphem 0.1";

Graphem::Graphem(int argc, char* argv[]) :
	QApplication(argc, argv),
	input(new InputWidget()),
	auth(new Auth(this)),
	settings(new QSettings("Graphem", "Graphem")),
	tries_left(0),
	print_pattern(false),
	verbose(false),
	lock_screen(false),
	status(-1)
{
	usage_total = settings->value("usage_total").toInt();
	usage_failed = settings->value("usage_failed").toInt();

	connect(input, SIGNAL(finished()),
		this, SLOT(authenticate()),
		Qt::QueuedConnection); //allow for repaint before checking

	connect(auth, SIGNAL(failed()),
		this, SLOT(failed()));
	connect(auth, SIGNAL(passed()),
		this, SLOT(passed()));

	for(int i = 1; i < argc; i++) {
		if(argv[i] == QString("--help")) {
			printHelp();
			status = 0;
			return;
		} else if(argv[i] == QString("--lock")) {
				lock_screen = true;
		} else if(argv[i] == QString("--pattern")) {
			if(i+1 >= argc)
				break; //parameter not found

			auth->setAuthPattern(argv[i+1]);
			i++;
#ifndef NO_DEBUG
		} else if(argv[i] == QString("--print-data")) {
			connect(input, SIGNAL(finished()),
				input, SLOT(printData()));
		} else if(argv[i] == QString("--print-pattern")) {
			print_pattern = true;
#endif
		} else if(argv[i] == QString("--show-input")) {
			input->showInput(true);
		} else if(argv[i] == QString("--touchpad")) {
			input->enableTouchpadMode(true);
		} else if(argv[i] == QString("--tries")) {
			if(i+1 >= argc)
				break; //parameter not found

			tries_left = QString(argv[i+1]).toInt();
			i++;
		} else if(argv[i] == QString("-v") or argv[i] == QString("--verbose")) {
			verbose = true;
		} else if(argv[i] == QString("--version")) {
			cout << qPrintable(version) << "\n";
			cout << "Copyright (C) 2009 Christian Pulvermacher\n";
			cout << "Documentation is available on http://graphem.berlios.de/\n";
			cout << "Using Qt " << qVersion();
			cout << ", compiled against Qt " << QT_VERSION_STR << "\n";
			status = 0;
			return;
		} else {
			cerr << "Unknown command line option '" << argv[i] << "'\n";
			printHelp();
			status = 1;
			return;
		}
	}
}


int Graphem::exec()
{
	if(status != -1) //status already set, exit
			return status;


	if(lock_screen) {
		input->setWindowTitle(version);
		input->setWindowFlags(Qt::X11BypassWindowManagerHint);
		input->setVisible(true);
		QDesktopWidget dw;
		input->setGeometry(dw.screenGeometry());

		input->grabKeyboard();
		input->grabMouse();
		input->activateWindow(); //make sure we catch keyboard events
		input->raise();
	} else { //show main window
		QWidget *main = new QWidget();
		main->setWindowTitle(version);
		QHBoxLayout *l1 = new QHBoxLayout();

		QVBoxLayout *l2 = new QVBoxLayout();
		info_text = new QTextEdit();
		info_text->setReadOnly(true);
		info_text->setFixedWidth(200);
		refreshInfo();

		QHBoxLayout *l3 = new QHBoxLayout();
		QPushButton *quit_button = new QPushButton("&Quit");
		connect(quit_button, SIGNAL(clicked()),
			this, SLOT(quit()));
		l3->addWidget(quit_button);
		l3->addWidget(new QPushButton("&New Pattern"));

		l2->addWidget(info_text);
		l2->addLayout(l3);

		l1->addLayout(l2);
		l1->addWidget(input);

		main->setLayout(l1);
		main->show();
	}

	return QApplication::exec();
}


void Graphem::failed()
{
	if(verbose)
		std::cout << "ERROR: Pattern not recognized.\n";
	usage_total++;
	usage_failed++;
	if(tries_left == 1) //this try was our last
		qApp->exit(1);

	if(tries_left != 0) //0 is for infinite amount of tries, don't decrease
		tries_left--;
	input->reset();
}


void Graphem::passed()
{
	if(verbose)
		std::cout << "OK: Correct pattern.\n";
	usage_total++;
	quit();
}


void Graphem::authenticate()
{
	auth->preprocess(input->path);
	if(print_pattern)
		auth->printPattern();
	auth->check();
}


void Graphem::printHelp()
{
	cout << "Usage: " << qPrintable(arguments().at(0)) << " [options]\n\n";
	cout << "--help\t\t\t Show this text\n";
	cout << "--lock\t\t\t Lock screen (Make sure your auth pattern works!)\n";
	cout << "--pattern [pattern]\t Specify a recorded pattern to check against\n";

#ifndef NO_DEBUG
	cout << "--print-data\t\t Prints velocity/pressure data to standard output\n";
	cout << "--print-pattern\t\t Prints entered pattern as a string\n";
#endif

	cout << "--show-input\t\t Shows input while drawing\n";
	cout << "--touchpad\t\t Touchpad mode, no clicking necessary\n";
	cout << "--tries [n]\t\t Exit Graphem with status code 1 after [n] tries; 0 to disable (default)\n";
	cout << "-v, --verbose\t\t Print success/failure messages on stdout\n";
	cout << "--version\t\t Print the version number and exit\n";
}


//refreshes info text on left side
void Graphem::refreshInfo()
{
	if(settings->value("pattern_hash").toString().isEmpty()) {
		info_text->setText("<h3>Welcome</h3>To start using Graphem, you have to set a new authentication pattern. Please click the \"New Pattern\" button.");
	} else {
		info_text->setText(QString("<h3>Statistics</h3> Total: ") + QString::number(usage_total) + "<br />Correct: " + QString::number(double(usage_total - usage_failed)/usage_total));
	}
}

void Graphem::quit()
{
	settings->setValue("usage_total", usage_total);
	settings->setValue("usage_failed", usage_failed);
	settings->sync();

	QApplication::quit();
}
