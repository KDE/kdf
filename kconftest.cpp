/* compile with
* gcc  -I$QTDIR/include -I$KDEDIR/include   -L/opt/kde/lib -L/usr/X11R6/lib -lkfm -lkdeui -lkdecore -lqt -lX11 -lXext -fno-rtti kconftest.cpp                
*/


#include <iostream>
#include <qstring.h>
#include <qdict.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>

static const char *description = 
	I18N_NOOP("A test application.");

static const char *version = "v0.0.1";

main(int argc, char ** argv)
{
 KCmdLineArgs::init(argc, argv, "test", description, version);
 
 KApplication app;
 KConfig * cfg = kapp->config();

 QDict<char> dict;

 dict.insert("Blah", "Arse");
 dict.insert("Blah", "Smack");
 dict.insert("Blah", "Monkey");

 QDictIterator<char> it(dict);

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
