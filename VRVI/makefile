	# Makefile for ASYNC-RBCD

CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
# directory of objective files
BUILDDIR := build
# directory of binary files
BINDIR := bin
# directory of source code
SRCDIR := src
# extension of source file
SRCEXT := cc
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

DEPENDENCY := $(shell find $(BUILDDIR) -type f -name *.d 2>/dev/null)

VRVI := $(BINDIR)/VRVI

CFLAGS := -g -std=c++0x -MMD -w 
LIB := -lgfortran -lpthread -lm -ansi
INC := -I include


all: $(VRVI)

$(VRVI): build/main.o
	@echo " $(CC) $^ -o $(VRVI) $(LIB)"; $(CC) $^ -o $(VRVI) $(LIB)
	@echo " $(VRVI) is successfully built."
	@printf '%*s' "150" | tr ' ' "-"
	@printf '\n'

# Compile code to objective files
###################################
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR) $(BINDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

run:
	./$(VRVI) -len_state 10 -len_action 10 -max_outer_loop 100 -max_inner_loop 100 -sample_num 100 -gamma 0.99
##############################################
clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(BINDIR)

-include $(DEPENDENCY)

