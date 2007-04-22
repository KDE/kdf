/*
**
** Copyright (C) 1999 by Michael Kropfberger
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/


/* compile with
* gcc  -I$QTDIR/include -I$KDEDIR/include   -L/opt/kde/lib -L/usr/X11R6/lib -lkfm -lkdeui -lkdecore -lqt -lX11 -lXext -fno-rtti kconftest.cpp                
*/


#include <iostream>

#include <q3dict.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kglobal.h>

static const char description[] =
	I18N_NOOP("A test application");

static const char version[] = "v0.0.1";

main(int argc, char ** argv)
{
 KCmdLineArgs::init(argc, argv, "test", description, version);
 
 KApplication app;
 KSharedConfig::Ptr cfg = KGlobal::config();

 Q3Dict<char> dict;

 dict.insert("Blah", "Arse");
 dict.insert("Blah", "Smack");
 dict.insert("Blah", "Monkey");

 Q3DictIterator<char> it(dict);

 QString key = "TestConfigItem";

 for (; it.current(); ++it) {

  cerr << "Before saving: " << endl;
  cerr << "key : \"" << key << "\"" << endl;
  cerr << "val : \"" << it.current() << "\"" << endl;

  debug("got back [%s]",cfg->writeEntry(key, it.current()));
//  debug("got back [%s]",s.data());

  cerr << "After saving: " << endl;
  cerr << "key : \"" << key << "\"" << endl;
  cerr << "val : \"" << it.current() << "\"" << endl;

  cerr << endl;
 }
}
