 CFLAGS = -I/usr/include/eigen3 -I/usr/include/opencv4 -lopencv_photo -lopencv_imgproc -lopencv_core -std=c++17 -O2
 
 all : inpaint
 .PHONY : all

inpaint : inpaint.o bilinear.o evaluate.o fast_marching.o generate.o nearest_neighbor.o utils.o
	cpp -o inpaint bilinear.o evaluate.o fast_marching.o generate.o nearest_neighbor.o utils.o $(CFLAGS)
inpaint.o : bilinear.h evaluate.h fast_marching.h generate.h nearest_neighbor.h utils.h RBF.hpp
	cpp -o inpaint.o -c main.cpp $(CFLAGS)
bilinear.o : utils.o defs.hpp kdtree.hpp utils.h
	cpp -o bilinear.o -c bilinear.cpp $(CFLAGS)

