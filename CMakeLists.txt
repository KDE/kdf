kde4_header()
project(kdf)
add_subdirectory( pics ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############
set(libkdf_common_SRCS 
	kdfwidget.cpp 
	kdfconfig.cpp 
	mntconfig.cpp 
	disklist.cpp
	disks.cpp 
	listview.cpp 
	optiondialog.cpp 
	stdoption.cpp )


set(kdf_SRCS kdf.cpp ${libkdf_common_SRCS} )

kde4_automoc(${kdf_SRCS})

kde4_add_executable(kdf ${kdf_SRCS})

target_link_libraries(kdf  ${KDE4_KDE3SUPPORT_LIBRARIES} )

install_targets(/bin kdf )


########### next target ###############

set(kwikdisk_SRCS kwikdisk.cpp ${libkdf_common_SRCS})

kde4_automoc(${kwikdisk_SRCS})

kde4_add_executable(kwikdisk ${kwikdisk_SRCS})

target_link_libraries(kwikdisk  ${KDE4_KDE3SUPPORT_LIBRARIES} )

install_targets(/bin kwikdisk )


########### next target ###############


########### next target ###############

set(kcm_kdf_PART_SRCS kcmdf.cpp ${libkdf_common_SRCS})

kde4_automoc(${kcm_kdf_PART_SRCS})

kde4_add_plugin(kcm_kdf ${kcm_kdf_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kcm_kdf )

target_link_libraries(kcm_kdf  ${KDE4_KDE3SUPPORT_LIBRARIES} )

install_targets(${LIB_INSTALL_DIR}/kde4 kcm_kdf )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kdf.desktop kwikdisk.desktop kcmdf.desktop )
install_files( ${DATA_INSTALL_DIR}/kdf FILES kdfui.rc )

kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = pics
#
#INCLUDES= $(all_includes)
#
#bin_PROGRAMS = kdf kwikdisk
#
#noinst_LTLIBRARIES = libkdf_common.la
#libkdf_common_la_SOURCES = kdfwidget.cpp kdfconfig.cpp mntconfig.cpp disklist.cpp \
#	disks.cpp listview.cpp optiondialog.cpp stdoption.cpp
#
#kdf_SOURCES = kdf.cpp 
#kdf_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kdf_LDADD = libkdf_common.la $(LIB_KFILE)
#
#kwikdisk_SOURCES = kwikdisk.cpp 
#kwikdisk_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kwikdisk_LDADD = libkdf_common.la $(LIB_KFILE)
#
#kde_module_LTLIBRARIES = kcm_kdf.la
#
#kcm_kdf_la_SOURCES = kcmdf.cpp
#kcm_kdf_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
#kcm_kdf_la_LIBADD = libkdf_common.la $(LIB_KFILE)
#
#noinst_HEADERS = kdfwidget.h kcmdf.h kdfconfig.h mntconfig.h \
#disks.h kdf.h disklist.h kwikdisk.h listview.h optiondialog.h stdoption.h
#
#METASOURCES = AUTO
#
#xdg_apps_DATA = kdf.desktop kwikdisk.desktop kcmdf.desktop
#
#rc_DATA = kdfui.rc
#
#rcdir = $(kde_datadir)/kdf
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kdf.pot
