obj := main
objects_ := Serializator utils
objects := $(addsuffix .o,$(objects_))

CXXFLAGS := -std=c++17 -Wfatal-errors

testDir := tests
testSrc := $(shell find $(testDir) -type f -name '*$(TEST_MASK)*.cpp' -printf '%f\n')
testNames := $(patsubst %.cpp,%,$(testSrc))
testNamesDir := $(addprefix $(testDir)/,$(testNames))

all: main

main: main.o $(objects)
	g++ $(CXXFLAGS) -g main.o -o main

main.o: main.cpp
	g++ -c $(CXXFLAGS) -g main.cpp -o $@

Serializator.o: Serializator.h types.h
	g++ -c $(CXXFLAGS) -g Serializator.h -o $@

utils.o: utils.cpp utils.h types.h
	g++ -c $(CXXFLAGS) -g utils.cpp -o $@

buildtest: $(testNamesDir)

tests/%: tests/%.cpp utils.o Serializator.h
	g++ $(CXXFLAGS) $< utils.o -o $@.exe

test: buildtest
	@cd $(testDir); \
	for f in $(testNames); do \
	  ./$$f > ./$${f}_actual.out; \
	  diff -qN $${f}_actual.out $${f}_expected.out > /dev/null && rm $${f}_actual.out || \
        { echo Test $${f} failed.; failed=1; diff -y $${f}_expected.out $${f}_actual.out; }; \
	done; \
	[[ -z "$$failed" ]] && { type beep.bat > /dev/null 2>&1 && beep.bat 2000 100; echo Tests passed.; }

clean:
	find -type f \( -name '*.exe' -o -name '*_actual.out' -o -name '*.o' \) -execdir rm '{}' \;

.PHONY: clean test buildtest all
