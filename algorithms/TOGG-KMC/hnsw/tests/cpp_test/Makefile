CXX ?= g++
CXXFLAGS += -O3 -march=native -std=c++11 -pthread -fPIC -fopenmp
CXXFLAGS += -I../../include/ -I../../third_party/spdlog/include/ -I../../third_party/googletest/googletest/ -I../../third_party/googletest/googletest/include/
LDFLAGS += -lpthread -L../../build/lib/static -ln2 -fopenmp

evaluation: evaluation.o
	$(CXX) -o $@ $? $(LDFLAGS)
	# $(CXX) -o $@ ../../build/obj/gtest-all.o ../../build/obj/gtest_main.o  $? $(LDFLAGS)

evaluation.o: evaluation.cpp
	$(CXX) $(CXXFLAGS) -c $?

clean:
	rm -f *.o evaluation
