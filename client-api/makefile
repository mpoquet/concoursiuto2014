MODULE_NAME := contest
JAVA_INCLUDE_DIR := /usr/lib/jvm/java-1.6.0-openjdk-i386/include
INCLUDES_DIR := $(shell pwd)/libnetwork/include $(shell pwd)/../server/
LIBS_DIR := $(shell pwd)/libnetwork/lib
LIBS := libnetwork

define join_and_prefix
    $(addprefix $(strip $1), $(subst $() $(),:,$(strip $2)))
endef

CXXFLAGS := $(addprefix -I, $(INCLUDES_DIR))
LDFLAGS := $(addprefix -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))
PYFLAGS := $(call join_and_prefix, -I, $(INCLUDES_DIR)) $(call join_and_prefix, -L, $(LIBS_DIR)) $(addprefix -l, $(LIBS))


	# General rules

all: clean c++ java python
	rm -rf build

test: clean c-test python-test java-test

clean:
	rm -rf build

clean-all: clean
	rm -rf bin

.PHONY: test clean clean-all c++ c-build c-test java java-build java-test python python-build python-test


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


	@# Python language


python: python-build

python-build:
	@echo "\n\tPYTHON BUILD:\n"
	mkdir -p build/python
	cd build/python && swig2.0 -c++ -outcurrentdir -python ../../c++/$(MODULE_NAME).i
	cp -R c++/* build/python
	cp -R python/* build/python
	cd build/python && python setup.py --quiet build_ext --inplace $(PYFLAGS)
	@#cd python && python setup.py --quiet build_ext --inplace
	mkdir -p bin/python
	cp build/python/$(MODULE_NAME).py build/python/_$(MODULE_NAME).* bin/python

python-test: python-build
	@echo "\n\tPYTHON TESTS:\n"
	cp python/test/test.py build/python
	cd build/python && python test.py

