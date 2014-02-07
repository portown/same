# Makefile

ifeq ($(RELEASE),1)
	CXXFLAGS += -O3
	LDFLAGS += -mwindows -static-libgcc -static-libstdc++
else
	CXXFLAGS += -g -O0
endif

CXXFLAGS += -pipe -std=c++11 -Wall -Wextra -pedantic-errors
CXXFLAGS += -I$(BOOST_ROOT)\include
LDFLAGS += -L$(BOOST_ROOT)\lib

MAIN_BIN = same.exe
MAIN_DIRS = same
MAIN_SOURCES = $(wildcard $(addsuffix /*.cxx,$(MAIN_DIRS)))
MAIN_OBJECTS = $(MAIN_SOURCES:.cxx=.o)
MAIN_DEPENDS = $(MAIN_SOURCES:.cxx=.d)
MAIN_LIBS = -lgdi32 -lshlwapi -lwinmm

MAIN_RESOURCES = same/resources.o
MAIN_RCS = same/same.rc
MAIN_RC_DEPENDS = same/resource.h

TEST_BIN = test.exe
TEST_SOURCES = $(wildcard sameTest/*.cxx)
TEST_OBJECTS = $(TEST_SOURCES:.cxx=.o)
TEST_DEPENDS = $(TEST_SOURCES:.cxx=.d)
TEST_LIBS = -lboost_unit_test_framework-mgw48-mt-1_55

.PHONY: all
all: $(MAIN_BIN)

.PHONY: check
check: $(TEST_BIN)
	./$(TEST_BIN)

.PHONY: test
test: check

.PHONY: clean
clean:
	rm -f $(MAIN_OBJECTS) $(MAIN_RESOURCES) $(TEST_OBJECTS)
	rm -f $(MAIN_DEPENDS) $(TEST_DEPENDS)
	rm -f $(MAIN_BIN) $(TEST_BIN)

$(MAIN_BIN): $(MAIN_OBJECTS) $(MAIN_RESOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(MAIN_LIBS)
ifeq ($(RELEASE),1)
	strip $@
endif

$(MAIN_RESOURCES): $(MAIN_RCS) $(MAIN_RC_DEPENDS)
	windres $< $@

$(TEST_BIN): $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(TEST_LIBS)
ifeq ($(RELEASE),1)
	strip $@
endif


%.o: %.cxx
	$(COMPILE.cpp) -c -o $@ $<


%.d: %.cxx
	$(CXX) $(CXXFLAGS) -MM $< > $@
	sed -i -e 's/\(^\S[^:]*\):/$(subst /,\/,$(dir $@))\1 $(subst /,\/,$@):/' $@
-include $(MAIN_DEPENDS) $(TEST_DEPENDS)
