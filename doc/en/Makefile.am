# just install datas here. Use install-exec-data for scripts and etc.
# the binary itself is already installed from automake
# use mkinstalldirs, not "install -d"
# don't install a list of file. Just one file per install.
# if you have more of them, create a subdirectory with an extra Makefile 
install-data-local: 
	$(mkinstalldirs) $(kde_htmldir)/en/kcontrol/kdf
	$(INSTALL_DATA) index.html $(kde_htmldir)/en/kcontrol/kdf
	$(mkinstalldirs) $(kde_htmldir)/en/kdf
	$(INSTALL_DATA) index.html $(kde_htmldir)/en/kdf

# remove ALL you have installed in install-data-local or install-exec-local
uninstall-local:
	-rm -f $(kde_htmldir)/en/kcontrol/kdf/index.html
	-rm -f $(kde_htmldir)/en/kdf/index.html

