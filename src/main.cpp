/*
  Main function.
  Copyright (C) 2005  Lucas Lorensi dos Santos

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(improlab);

    QApplication app(argc, argv);
    MdiMain mainWin;
    mainWin.show();
    return app.exec();
}
