SUBDIRS = pics

INCLUDES= $(all_includes)

bin_PROGRAMS = kdf kwikdisk

kdf_SOURCES = kdf.cpp kdfwidget.cpp kdfconfig.cpp mntconfig.cpp \
disklist.cpp disks.cpp listview.cpp optiondialog.cpp stdoption.cpp

kdf_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kdf_LDADD = $(LIB_KFILE)

kwikdisk_SOURCES = kwikdisk.cpp kdfconfig.cpp mntconfig.cpp \
disklist.cpp disks.cpp listview.cpp optiondialog.cpp stdoption.cpp

kwikdisk_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwikdisk_LDADD = $(LIB_KFILE)

kde_module_LTLIBRARIES = kcm_kdf.la

kcm_kdf_la_SOURCES = kcmdf.cpp kdfwidget.cpp kdfconfig.cpp mntconfig.cpp \
	disklist.cpp disks.cpp listview.cpp optiondialog.cpp stdoption.cpp

kcm_kdf_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
kcm_kdf_la_LIBADD = $(LIB_KFILE)

noinst_HEADERS = kdfwidget.h kcmdf.h kdfconfig.h mntconfig.h \
disks.h kdf.h disklist.h kwikdisk.h listview.h optiondialog.h stdoption.h

METASOURCES = AUTO

desktopdir = $(kde_appsdir)/System
desktop_DATA = kdf.desktop kwikdisk.desktop

kcmdesktopdir = $(kde_appsdir)/Settings/Information
kcmdesktop_DATA = kcmdf.desktop

messages:
	$(XGETTEXT) *.cpp -o $(podir)/kdf.pot
