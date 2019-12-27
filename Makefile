CXX = g++
CXXFLAGS = -g
RM = rm

cache: cache.o
	$(CXX) $(CXXFLAGS) -o cache cache.o

cache.o: cache.cpp
	$(CXX) $(CXXFLAGS) -c cache.cpp

clean:
	$(RM) *.o cache
