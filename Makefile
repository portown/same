# Makefile

include Makefile.config

SUBDIRS = src test

.PHONY: all
all: $(MAIN_BIN)

.PHONY: $(MAIN_BIN)
$(MAIN_BIN):
	$(MAKE) -C src $@
	mv src/$@ .
	cp /usr/local/mingw/x86_64-w64-mingw32/lib/libstdc++-6.dll /usr/local/mingw/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll .

.PHONY: check
check:
	$(MAKE) -C test $@

.PHONY: clean
clean:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) $@;)
	rm -f $(MAIN_BIN)
