#
#  Makefile for fltk applications
#

LIBS = -L/usr/lib/nvidia-352 -lGL -lGLU -ljpeg -lpng -lz

CFLAGS = -g -O2 -std=c++11 -pthread $(INCLUDE) $(LIBS) -fopenmp

CC = g++

.SUFFIXES: .o .cpp .cxx

.cpp.o: 
	$(CC) $(CFLAGS) -c -o $*.o $<

.cxx.o: 
	$(CC) $(CFLAGS) -c -o $*.o $<

ALL.O = src/main.o src/getopt.o src/RayTracer.o \
      	src/ui/CommandLineUI.o \
	src/ui/ModelerCamera.o \
	src/fileio/bitmap.o src/fileio/buffer.o \
	src/fileio/pngimage.o \
	src/parser/Token.o src/parser/Tokenizer.o \
	src/parser/Parser.o src/parser/ParserException.o \
	src/scene/camera.o src/scene/light.o\
	src/scene/material.o src/scene/ray.o src/scene/scene.o \
	src/SceneObjects/Box.o src/SceneObjects/Cone.o \
	src/SceneObjects/Cylinder.o src/SceneObjects/trimesh.o \
	src/SceneObjects/Sphere.o src/SceneObjects/Square.o

ray: $(ALL.O)
	$(CC) $(CFLAGS) -o $@ $(ALL.O) $(LIBS)

clean:
	rm -f $(ALL.O) *.bmp *.report

clean_all:
	rm -f $(ALL.O) ray

