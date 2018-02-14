# Makefile

include ../Makefile.config

CXXFLAGS += -I../src

.PHONY: test
test: $(TEST_BIN)

.PHONY: check
check: $(TEST_BIN)
	./$(TEST_BIN)

.PHONY: clean
clean:
	rm -f $(TEST_OBJECTS)
	rm -f $(TEST_DEPENDS)
	rm -f $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(TEST_LIBS)
ifeq ($(RELEASE),1)
	$(STRIP) $@
endif


%.o: %.cxx
	$(COMPILE.cpp) -c -o $@ $<


%.d: %.cxx
	$(CXX) $(CXXFLAGS) -MM $< > $@
	sed -i -e 's/\(^\S[^:]*\):/$(subst /,\/,$(dir $@))\1 $(subst /,\/,$@):/' $@
-include $(TEST_DEPENDS)
