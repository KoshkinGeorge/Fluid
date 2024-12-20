ADDITIONAL = ./Additional

CPP_C = g++
CPP_FLAGS = -std=c++20 -Wall -Wextra -O4

TYPES = FIXED(32,16),FLOAT,FAST_FIXED(53,11)
# TYPES = "FIXED(32,16)"
SIZES = S(14,5),S(36,84)
ARGS = "--p-type = FLOAT" "--v-type =   	FAST_FIXED( 53, 		11)" "--v-flow-type=FIXED(32,16)" "in.bin"
# ARGS = "FIXED(32,16)" "FIXED(32,16)" "FIXED(32,16)" "in.bin"

SOURCE = main.cpp
OBJECTS = ${SOURCE:.cpp=.o} 
EXECUTABLE = main

all: build
	./${EXECUTABLE} ${ARGS}


build: ${OBJECTS}
	${CPP_C} ${CPP_FLAGS} ${OBJECTS} -o ${EXECUTABLE}

%.o: %.cpp
	${CPP_C} ${CPP_FLAGS} -c $< -o $@

main.o: main.cpp
	${CPP_C} ${CPP_FLAGS} -DTYPES="${TYPES}" -DSIZES="${SIZES}" -c $< -o $@

fluid: ${ADDITIONAL}/fluid.cpp
	${CPP_C} ${CPP_FLAGS} ${ADDITIONAL}/fluid.cpp -o ${ADDITIONAL}/fluid
	${ADDITIONAL}/fluid

clear:
	rm -f *.o
	rm -f main
	rm -f ${ADDITIONAL}/fluid

load-bin: ${ADDITIONAL}/in_helper.cpp
	g++ $< -o ${ADDITIONAL}/helper
	${ADDITIONAL}/helper