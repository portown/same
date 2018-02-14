# Makefile

include ../Makefile.config

.PHONY: clean
clean:
	rm -f $(MAIN_OBJECTS) $(MAIN_RESOURCES)
	rm -f $(MAIN_DEPENDS)
	rm -f $(MAIN_BIN)

$(MAIN_BIN): $(MAIN_OBJECTS) $(MAIN_RESOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(MAIN_LIBS)
ifeq ($(RELEASE),1)
	strip $@
endif

$(MAIN_RESOURCES): $(MAIN_RCS) $(MAIN_RC_DEPENDS)
	windres $< $@


%.o: %.cxx
	$(COMPILE.cpp) -c -o $@ $<


%.d: %.cxx
	$(CXX) $(CXXFLAGS) -MM $< > $@
	sed -i -e 's/\(^\S[^:]*\):/$(subst /,\/,$(dir $@))\1 $(subst /,\/,$@):/' $@
-include $(MAIN_DEPENDS)
