kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kdf/pics FILES tick.png delete.png )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#KDE_ICON = kdf kcmdf kwikdisk
#pics_DATA = tick.png delete.png
#picsdir = $(kde_datadir)/kdf/pics
#
