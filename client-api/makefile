MODULE_NAME := contest
JAVA_INCLUDE_DIR := /usr/lib/jvm/java-1.6.0-openjdk-i386/include
INCLUDES_DIR := $(shell pwd)/spnl-install/include/spnl $(shell pwd)/../server/
LIBS_DIR := $(shell pwd)/spnl-install/lib
LIBS := spnl

define join_and_prefix
    $(addprefix $(strip $1), $(subst $() $(),:,$(strip $2)))
endef

CXXFLAGS := -fPIC $(addprefix -I, $(INCLUDES_DIR))
LDFLAGS := $(addprefix -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))
PYFLAGS := $(call join_and_prefix, -I, $(INCLUDES_DIR)) $(call join_and_prefix, -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))


	# General rules

all: clean c++ java python2 python3 clean

spnl:
	mkdir -p build/spnl
	cd build/spnl && cmake ../../spnl -DCMAKE_INSTALL_PREFIX:PATH=../../spnl-install
	cd build/spnl && make install -j4 -s

test: clean c-test python2-test java-test

clean:
	rm -rf build

clean-all: clean
	rm -rf bin
	rm -rf spnl-install

.PHONY: test spnl clean clean-all c++ c-build c-test java java-build java-test python2 python2-build python2-test python3 python3-build


	@# C++ language


c: c-build

c++-build: 
	@echo "\n\tC BUILD:\n"
	mkdir -p build/c++
	cp -R c++/* build/c
	cd build/c++ && $(CXX) -c *.cpp $(CXXFLAGS)
	cd build/c++ && $(CXX) -shared *.o -o lib$(MODULE_NAME).so $(LDFLAGS)
	mkdir -p bin/c++
	cp build/c++/lib$(MODULE_NAME).* bin/c++

c++-test: c-build
	@echo "\n\tC TESTS:\n"
	@# [...] 


	@# Java language


java: java-build

java-build:
	@echo "\n\tJAVA BUILD:\n"
	mkdir -p build/java
	cd build/java && swig2.0 -c++ -outcurrentdir -java ../../c++/$(MODULE_NAME).i
	cp -R c++/* build/java
	cp -R java/* build/java
	cd build/java && $(CXX) -c *.cpp *.cxx -I$(JAVA_INCLUDE_DIR) $(CXXFLAGS)
	cd build/java && $(CXX) -shared *.o -o lib$(MODULE_NAME).so $(LDFLAGS)
	cd build/java && javac *.java
	cd build/java && jar cvf $(MODULE_NAME).jar *.class *.so > /dev/null
	cd build/java && rm -f *.class *.o *.c *.i
	mkdir -p bin/java
	cp build/java/$(MODULE_NAME).jar build/java/lib$(MODULE_NAME).* bin/java

java-test: java-build
	@echo "\n\tJAVA TESTS:\n"
	@# [...] javac -cp $(MODULE_NAME).jar test.java


	@# Python 2 language


python2: python2-build

python2-build:
	@echo "\n\tPYTHON-2 BUILD:\n"
	mkdir -p build/python2
	cd build/python2 && swig2.0 -c++ -outcurrentdir -python ../../c++/$(MODULE_NAME).i
	cp -R c++/* build/python2
	cp -R python2/* build/python2
	cd build/python2 && python2 setup.py --quiet build_ext --inplace $(PYFLAGS)
	mkdir -p bin/python2
	cp build/python2/$(MODULE_NAME).py build/python2/_$(MODULE_NAME).* bin/python2

python2-test: python2-build
	@echo "\n\tPYTHON-2 TESTS:\n"
	cp python2/test/test.py build/python2
	cd build/python2 && python2 test.py


	@# Python 3 language


python3: python3-build

python3-build:
	@echo "\n\tPYTHON-3 BUILD:\n"
	mkdir -p build/python3
	cd build/python3 && swig2.0 -c++ -outcurrentdir -py3 -python ../../c++/$(MODULE_NAME).i
	cp -R c++/* build/python3
	cp -R python3/* build/python3
	cd build/python3 && python3 setup.py --quiet build_ext --inplace $(PYFLAGS)
	mkdir -p bin/python3
	cp build/python3/$(MODULE_NAME).py build/python3/_$(MODULE_NAME).* bin/python3

python3-test: python3-build
	@echo "\n\tPYTHON-3 TESTS:\n"
