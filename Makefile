CPP = g++
CFLAGS = -I/usr/include/opencv4 -std=c++17 -O2
 
all : inpaint
.PHONY : all

inpaint : inpaint.o bilinear.o evaluate.o fast_marching.o generate.o nearest_neighbor.o utils.o
	$(CPP) -o inpaint inpaint.o bilinear.o evaluate.o fast_marching.o generate.o nearest_neighbor.o utils.o $(CFLAGS) -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_photo
inpaint.o : bilinear.h evaluate.h fast_marching.h generate.h nearest_neighbor.h utils.h RBF.hpp
	$(CPP) -o inpaint.o -c main.cpp $(CFLAGS)
bilinear.o : utils.o defs.hpp kdtree.hpp utils.h
	$(CPP) -o bilinear.o -c bilinear.cpp $(CFLAGS) -I/usr/include/eigen3
evaluate.o : defs.hpp
	$(CPP) -o evaluate.o -c evaluate.cpp $(CFLAGS)
fast_marching.o : defs.hpp
	$(CPP) -o fast_marching.o -c fast_marching.cpp $(CFLAGS)
generate.o : utils.o defs.hpp generate.h utils.h
	$(CPP) -o generate.o -c generate.cpp $(CFLAGS)
nearest_neighbor.o : utils.o defs.hpp kdtree.hpp
	$(CPP) -o nearest_neighbor.o -c nearest_neighbor.cpp $(CFLAGS)
utils.o : utils.h
	$(CPP) -o utils.o -c utils.cpp $(CFLAGS)

.PHONY : clean
clean :
	-rm inpaint *.o
