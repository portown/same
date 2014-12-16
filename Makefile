# Makefile

ifeq ($(RELEASE),1)
	CXXFLAGS += -O3 -DNDEBUG -D_NDEBUG
	LDFLAGS += -mwindows -static-libgcc -static-libstdc++
else
	CXXFLAGS += -g -O0 -DDEBUG -D_DEBUG
endif

CXXFLAGS += -pipe -std=c++11 -Wall -Wextra -pedantic-errors

MAIN_BIN = same.exe
MAIN_DIRS = src
MAIN_SOURCES = $(wildcard $(addsuffix /*.cxx,$(MAIN_DIRS)))
MAIN_OBJECTS = $(MAIN_SOURCES:.cxx=.o)
MAIN_DEPENDS = $(MAIN_SOURCES:.cxx=.d)
MAIN_LIBS = -lgdi32 -lshlwapi -lwinmm

MAIN_RESOURCES = src/resources.o
MAIN_RCS = src/same.rc
MAIN_RC_DEPENDS = src/resource.h

TEST_BIN = test.exe
TEST_DIRS = test
TEST_SOURCES = $(wildcard $(addsuffix /*.cxx,$(TEST_DIRS)))
TEST_OBJECTS = $(TEST_SOURCES:.cxx=.o)
TEST_DEPENDS = $(TEST_SOURCES:.cxx=.d)
TEST_LIBS = -lCppUTest -lCppUTestExt

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
