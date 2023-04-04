EX_NAME = v_x_heatmap
CPP_FLAGS = -c -std=c++14 -O1

OBJS = velocity_distribution.o

$(EX_NAME) : $(OBJS)
	g++ -o $(EX_NAME) $(OBJS) -lstdc++fs

velocity_distribution.o : velocity_distribution.cpp
	g++ $(CPP_FLAGS) velocity_distribution.cpp -lstdc++fs

clean:
	rm -f core $(EX_NAME) $(OBJS)
