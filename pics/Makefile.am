
kde_icon_DATA = kdf.xpm kcmdf.xpm kwikdisk.xpm

kde_mini_DATA = nfs_mount.xpm nfs_unmount.xpm

# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kdf.xpm $(kde_minidir)/kdf.xpm
	$(INSTALL_DATA) $(srcdir)/mini-kcmdf.xpm $(kde_minidir)/kcmdf.xpm
	$(INSTALL_DATA) $(srcdir)/mini-kwikdisk.xpm $(kde_minidir)/kwikdisk.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_minidir)/kdf.xpm
	-rm -f $(kde_minidir)/kcmdf.xpm
	-rm -f $(kde_minidir)/kwikdisk.xpm


