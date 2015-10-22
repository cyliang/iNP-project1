RAS_ROOT?=$(shell echo $$HOME)/ras
RAS_BIN=$(RAS_ROOT)/bin

CC:=$(CXX)
CPPFLAGS='-DRAS_ROOT="$(abspath $(RAS_ROOT))"'
OUTPUT_OPTION=-MMD -MP -o $@
LDLIBS=

######################## Source Code Compilation ##########################
all: ras_server shell

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
DEP=$(SRC:.cpp=.d)
-include $(DEP)

.PHONY: clean

ras_server: ras_server.o

shell: $(filter-out ras_server.o, $(OBJ))

clean:
	rm -f $(OBJ) $(DEP) ras_server shell
###########################################################################


######################## Environment Compilation ##########################
CMD_TARGETS=$(addprefix $(RAS_BIN)/, $(basename $(notdir $(wildcard commands/*.cpp))))

.PHONY: commands clean_commands

commands: $(RAS_BIN) $(CMD_TARGETS)

$(RAS_BIN):
	mkdir $(RAS_BIN)

$(RAS_BIN)/%: commands/%.cpp
	$(CXX) -o $@ $<

clean_commands:
	-rm -f $(RAS_BIN)/* $(RAS_ROOT)/*
###########################################################################
