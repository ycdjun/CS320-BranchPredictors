CC=gcc
CXX=g++
RM=rm -f

SRCS=predictors.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: predictors

predictors: $(OBJS)
	$(CXX) -o predictors $(OBJS)

predictors.o: predictors.cpp

clean:
	$(RM) $(OBJS)

