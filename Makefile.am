
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

# just set the variable
kdf_APPSDIR = $(kde_appsdir)/System
kwikdisk_APPSDIR = $(kde_appsdir)/System
kcmdf_APPSDIR = $(kde_appsdir)/Settings/Information
# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
SUBDIRS = pics

bin_PROGRAMS = 	kcmdf kdf kwikdisk

# Which sources should be compiled
kdf_SOURCES	= kdf.cpp kdfwidget.cpp kdfconfig.cpp mntconfig.cpp disklist.cpp disks.cpp
kcmdf_SOURCES	= kcmdf.cpp kdfwidget.cpp kdfconfig.cpp mntconfig.cpp disklist.cpp disks.cpp
kwikdisk_SOURCES	= kwikdisk.cpp kdfconfig.cpp mntconfig.cpp disklist.cpp disks.cpp

# the library search path
kdf_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kcmdf_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kwikdisk_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# the libraries to link against. Be aware of the order. First the libraries,
# that depend on the following ones.
kdf_LDADD   = $(LIB_KFILE)
kcmdf_LDADD   = $(LIB_KFILE)
kwikdisk_LDADD   = $(LIB_KFILE)

# this option you can leave out. Just, if you use "make dist", you need it
noinst_HEADERS = kdfwidget.h  kcmdf.h kdfconfig.h  mntconfig.h disks.h kdf.h  disklist.h kwikdisk.h

# just to make sure, automake makes them 
METASOURCES =	kcmdf.moc kdf.moc kdfwidget.moc kdfconfig.moc  mntconfig.moc disks.moc disklist.moc kwikdisk.moc

# make messages.po. Move this one to ../po/ and "make merge" in po
messages:
	$(XGETTEXT) -C -ktranslate -ki18n $(kdf_SOURCES) $(kcmdf_SOURCES) $(kwikdisk_SOURCES) && mv messages.po ../po/kdf.po

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kdf_APPSDIR)
	$(mkinstalldirs) $(kcmdf_APPSDIR)
	$(mkinstalldirs) $(kwikdisk_APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kdf.kdelnk $(kdf_APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kcmdf.kdelnk $(kcmdf_APPSDIR)
	$(INSTALL_DATA) $(srcdir)/kwikdisk.kdelnk $(kwikdisk_APPSDIR)

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kdf_APPSDIR)/kdf.kdelnk
	-rm -f $(kcmdf_APPSDIR)/kcmdf.kdelnk
	-rm -f $(kwikdisk_APPSDIR)/kwikdisk.kdelnk

