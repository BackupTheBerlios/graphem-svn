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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class InputWidget;
class NewPattern;
class QAction;
class QDockWidget;
class QTextEdit;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(InputWidget *input);
public slots:
	void refreshInfo();
	void reset();
	void save();
	void showAboutDialog();
	void showEditPatternDialog();
	void showNewPatternDialog();
	void showPreferences();
	void quit();
private:
	void setUnsavedChanges(bool b);

	InputWidget* input;
	NewPattern* new_pattern_dialog;
	QDockWidget* info_dock;
	QTextEdit* info_text;
	QAction* edit_action;
	QAction* save_action;
	bool unsaved_changes;
};
#endif
