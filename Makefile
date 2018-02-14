# Makefile

include Makefile.config

SUBDIRS = src test

.PHONY: all
all: $(MAIN_BIN)

.PHONY: $(MAIN_BIN)
$(MAIN_BIN):
	$(MAKE) -C src $@
	mv src/$@ .

.PHONY: check
check:
	$(MAKE) -C test $@

.PHONY: clean
clean:
	$(foreach dir,$(SUBDIRS),$(MAKE) -C $(dir) $@;)
	rm -f $(MAIN_BIN)
