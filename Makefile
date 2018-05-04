CPPFLAGS = -std=c++11
CXX=mpiCC
LDFLAGS=-lboost_mpi -lboost_serialization

source = sendstring.cpp sendclass1.cpp send_from_all.cpp multiple_communicators.cpp vector_sum_scatter.cpp 
executables = $(source:.cpp=)

%: %.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $@.cpp $(LDFLAGS) -o $@

all: $(executables)

.phony: clean rcs

clean:
		rm -f $(executables) sendclass1_from1.out

rcs:
		ci -l $(source) Makefile
