SUBDIRS = pics

INCLUDES= $(all_includes)

bin_PROGRAMS = kdf kwikdisk

noinst_LTLIBRARIES = libkdf_common.la
libkdf_common_la_SOURCES = kdfwidget.cpp kdfconfig.cpp mntconfig.cpp disklist.cpp \
	disks.cpp listview.cpp optiondialog.cpp stdoption.cpp

kdf_SOURCES = kdf.cpp 
kdf_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kdf_LDADD = libkdf_common.la $(LIB_KFILE)

kwikdisk_SOURCES = kwikdisk.cpp 
kwikdisk_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwikdisk_LDADD = libkdf_common.la $(LIB_KFILE)

kde_module_LTLIBRARIES = kcm_kdf.la

kcm_kdf_la_SOURCES = kcmdf.cpp
kcm_kdf_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_kdf_la_LIBADD = libkdf_common.la $(LIB_KFILE)

noinst_HEADERS = kdfwidget.h kcmdf.h kdfconfig.h mntconfig.h \
disks.h kdf.h disklist.h kwikdisk.h listview.h optiondialog.h stdoption.h

METASOURCES = AUTO

xdg_apps_DATA = kdf.desktop kwikdisk.desktop kcmdf.desktop

rc_DATA = kdfui.rc

rcdir = $(kde_datadir)/kdf

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kdf.pot
