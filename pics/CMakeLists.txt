



########### install files ###############

install( FILES tick.png delete.png  DESTINATION ${DATA_INSTALL_DIR}/kdf/pics)

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )




#original Makefile.am contents follow:

#KDE_ICON = kdf kcmdf kwikdisk
#pics_DATA = tick.png delete.png
#picsdir = $(kde_datadir)/kdf/pics
#
