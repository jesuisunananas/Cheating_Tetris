COMPILER = clang++
CPPFLAGS = -std=c++17
SOURCE_LIBS = -Ilib/
OSX_OPT = -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
OSX_OUT = -o "bin/build_osx"
CFILES = src/*.cpp

build_osx:
	$(COMPILER) $(CPPFLAGS) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)