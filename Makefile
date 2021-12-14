ifeq (, $(shell which 2>/dev/null g++-11))
	CC = g++
else
	CC = g++-11
endif

CC_FLAGS = -c -g -std=c++20 -Wall -I src/include -fmodules-ts
LD_FLAGS = -ltoxcore -lncurses -ltinfo -pthread

PROJ_NAME = tfg

# .cpp files
#SOURCES := $(shell find . -name '*.cpp')
SOURCES := $(wildcard src/**/*.cpp)
HEADERS := $(wildcard src/**/*.h)

SOURCES += $(wildcard src/*.cpp)
HEADERS += $(wildcard src/*.h)

# Object files
OBJ=$(addprefix objects/,$(subst .cpp,.o,$(SOURCES:src/%=%)))

# Command used at clean target
RM = rm -rf

# Compilation and linking
all: objFolder $(PROJ_NAME)
	@ echo 'Done!'

$(PROJ_NAME): $(OBJ)
	@ echo 'Building binary using GCC linker: $@'
	$(CC) $^ -o $@ $(LD_FLAGS)
	@ echo 'Finished building binary: $@'
	@ echo ' '

objects/%.o: ./src/%.cpp
	@mkdir -p $(dir $@)
	@ echo 'Building target using GCC compiler: $<'
	$(CC) $< $(CC_FLAGS) $(LD_FLAGS) -o $@
	@ echo ' '

objFolder:
	@ mkdir -p objects

clean:
	@ $(RM) ./objects/* $(PROJ_NAME) *~
	@ rmdir objects

.PHONY: all clean