
docs_DATA = index-1.html index-2.html index.html 
docsdir = $(kde_htmldir)/en/kcontrol/kdf

install-data-local:
	rm -rf $(kde_htmldir)/en/kdf
	$(LN_S) $(DESTDIR)$(kde_htmldir)/en/kcontrol/kdf $(DESTDIR)$(kde_htmldir)/en/kdf 

uninstall-local:
	rm -rf $(kde_htmldir)/en/kdf

