# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kde_icondir)
	$(INSTALL_DATA) kdf.xpm $(kde_icondir)
	$(INSTALL_DATA) kcmdf.xpm $(kde_icondir)
	$(INSTALL_DATA) kwikdisk.xpm $(kde_icondir)
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kdf.xpm $(kde_minidir)/kdf.xpm
	$(INSTALL_DATA) mini-kcmdf.xpm $(kde_minidir)/kcmdf.xpm
	$(INSTALL_DATA) mini-kwikdisk.xpm $(kde_minidir)/kwikdisk.xpm
#device-type icons
	$(INSTALL_DATA) nfs_mount.xpm $(kde_minidir)/nfs_mount.xpm
	$(INSTALL_DATA) nfs_unmount.xpm $(kde_minidir)/nfs_unmount.xpm

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_icondir)/kdf.xpm
	-rm -f $(kde_icondir)/kcmdf.xpm
	-rm -f $(kde_icondir)/kwikdisk.xpm
	-rm -f $(kde_minidir)/kdf.xpm
	-rm -f $(kde_minidir)/kcmdf.xpm
	-rm -f $(kde_minidir)/kwikdisk.xpm
#device-type icons
	-rm -f $(kde_minidir)/nfs_mount.xpm
	-rm -f $(kde_minidir)/nfs_unmount.xpm

