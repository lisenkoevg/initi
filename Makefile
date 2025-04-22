obj := main
CXXFLAGS := -std=c++17

testDir := tests
testSrc := $(shell find $(testDir) -type f -name '*.cpp' -printf '%f\n')
testNames := $(patsubst %.cpp,%,$(testSrc))
testNamesDir := $(addprefix $(testDir)/,$(testNames))

all: main

mai%: mai%.cpp Serializator.cpp
	g++ $(CXXFLAGS) -g $< -o $@

buildtest: $(testNamesDir)

tests/%: tests/%.cpp Serializator.cpp
	g++ $(CXXFLAGS) $@.cpp -o $@.exe

test: buildtest
	@cd $(testDir); for f in $(testNames); do \
      ./$$f > ./$${f}_actual.out; \
      diff -qN $${f}_actual.out $${f}_expected.out > /dev/null && rm $${f}_actual.out || { echo Test $${f} failed.; failed=1; }; \
    done; \
    [[ -z "$$failed" ]] && echo Tests passed.

clean:
	find -type f \( -name '*.exe' -o -name '*_actual.out' \) -execdir rm '{}' \;

.PHONY: clean test buildtest all
