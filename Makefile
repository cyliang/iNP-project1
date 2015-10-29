RAS_ROOT?=$(shell echo $$HOME)/ras
TEST_DATA?=test_data
RAS_BIN=$(RAS_ROOT)/bin

CC:=$(CXX)
CXXFLAGS='-DRAS_ROOT="$(abspath $(RAS_ROOT))"'
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

############################ Test Environment #############################
.PHONY: test
test: clean_commands commands
	cp $(TEST_DATA)/test.html $(RAS_ROOT)
###########################################################################

######################## Environment Compilation ##########################
CMD_TARGETS=$(addprefix $(RAS_BIN)/, $(basename $(notdir $(wildcard commands/*.cpp))))

.PHONY: commands clean_commands

commands: $(RAS_BIN) $(CMD_TARGETS)
	cp /bin/{cat,ls} $(RAS_BIN)

$(RAS_BIN):
	mkdir $(RAS_BIN)

$(RAS_BIN)/%: commands/%.cpp
	$(CXX) -o $@ $<

clean_commands:
	-rm -f $(RAS_BIN)/* $(RAS_ROOT)/*
###########################################################################
