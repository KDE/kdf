
########### install files ###############

install( PROGRAMS org.kde.kdf.desktop org.kde.kwikdisk.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install( FILES kdfui.rc  DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/kdf)
install(FILES org.kde.kdf.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
