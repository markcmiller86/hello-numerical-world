# ./heat alpha=8.2e-10 lenx=0.25 dx=0.01 dt=100 maxt=5580000 outi=100 savi=1000 bc0=233.15 bc1=294.261 ic="const(294.261)"
# make CXX=clang CXXFLAGS=-fopenmp LDFLAGS="-lomp -lstdc++" heat
# macOS...
# https://mac.r-project.org/openmp/
# make CXX=clang CXXFLAGS="-Xclang -fopenmp -I/Users/miller86/ideas-ecp/hello-numerical-world/omp/include" LDFLAGS="-lomp -lstdc++" heat
# make CXXFLAGS="-Xpreprocessor -fopenmp" CPPFLAGS=-DFPTYPE=0 LDFLAGS=-lomp ERRBND=1e-1 check_all
# had to copy libomp.dylib to /usr/local/lib
ERRBND ?= 1e-6
PTOOL ?= visit
RUNAME ?= heat_results
PIPEWIDTH ?= 0.1
RM = rm

# Headers
HDR = Number.h heat.h
# Source Files
SRC = heat.c utils.c args.c exact.c ftcs.c crankn.c dufrank.c
# Object Files
OBJ = $(SRC:.c=.o)
# Coverage Files
GCOV = $(SRC:.c=.c.gcov) $(SRC:.c=.gcda) $(SRC:.c=.gcno) $(HDR:.h=.h.gcov)
# Executable
EXE = heat

# Implicit rule for object files
%.o : %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# Help is default target
help:
	@echo "Targets:"
	@echo "    heat: makes the default heat application (double precision)"
	@echo "    heat-omp: makes default heat application with openmp threads"
	@echo "    heat-single: makes the heat application with single precision" 
	@echo "    heat-double: makes the heat application with double precision" 
	@echo "    heat-long-double: makes the heat application with long-double precision" 
	@echo "    PTOOL=[gnuplot,matplotlib,visit] RUNAME=<run-dir-name> plot: plots results"
	@echo "    check: runs various tests confirming steady-state is linear"


# Linking the final heat app
heat: $(OBJ)
	$(CC) -o heat $(OBJ) $(LDFLAGS) -lm

heat-omp: CC=clang
heat-omp: CFLAGS=-fopenmp
heat-omp: LDFLAGS=-lomp -lstdc++
heat-omp: $(OBJ)
heat-omp: heat
	mv heat heat-omp

# All objects depend on header
$(OBJ): $(HDR)

# Convenience variable/target for half-precision
heat-half: CPPFLAGS=-DFPTYPE=0
heat-half: CFLAGS=-Wno-format
heat-half: $(OBJ)
heat-half: heat
	mv heat heat-half

# Convenience variable/target for single-precision
heat-single: CPPFLAGS=-DFPTYPE=1
heat-single: $(OBJ)
heat-single: heat
	mv heat heat-single

# Convenience variable/target for double-precision
# Same as default
heat-double: CPPFLAGS=-DFPTYPE=2
heat-double: $(OBJ)
heat-double: heat
	mv heat heat-double

# Convenience variable/target for long-double-precision
heat-long-double: CPPFLAGS=-DFPTYPE=3
heat-long-double: $(OBJ)
heat-long-double: heat
	mv heat heat-long-double

# convenient target to plot results
plot:
	@test -r ./tools/run_$(PTOOL).sh || ( echo "Cannot find plotting tool \"$(PTOOL)\"" && exit 1 )
	@test -d $(RUNAME) || ( echo "Cannot find results dir \"$(RUNAME)\"" && exit 1 )
	@test -d $(RUNAME) && ./tools/run_$(PTOOL).sh $(RUNAME) $(PIPEWIDTH)

check_clean:
	$(RM) -rf check check_crankn check_dufrank
	$(RM) -rf heat heat-omp heat-single heat-double heat-long-double

clean: check_clean
	$(RM) -f $(OBJ) $(EXE) $(GCOV)

#
# Run for a long time with random initial condition
# and confirm linear stead-state upon termination
#
check/check_soln_final.curve:
	./heat runame=check outi=0 maxt=10 ic="rand(0,0.2,2)"

check: heat check/check_soln_final.curve
	@echo "Time zero..."
	@cat check/check_soln_00000.curve
	@echo "Final result..."
	@cat check/check_soln_final.curve
	./check_lss.sh check/check_soln_final.curve $(ERRBND)

check_ftcs: check

check_crankn/check_crankn_soln_final.curve:
	./heat alg=crankn runame=check_crankn outi=0 maxt=10 ic="rand(0,0.2,2)"

check_crankn: heat check_crankn/check_crankn_soln_final.curve
	cat check_crankn/check_crankn_soln_final.curve
	./check_lss.sh check_crankn/check_crankn_soln_final.curve $(ERRBND)

check_dufrank/check_dufrank_soln_final.curve:
	./heat alg=dufrank runame=check_dufrank outi=0 maxt=40 ic="rand(0,0.2,2)"

check_dufrank: heat check_dufrank/check_dufrank_soln_final.curve
	cat check_dufrank/check_dufrank_soln_final.curve
	./check_lss.sh check_dufrank/check_dufrank_soln_final.curve $(ERRBND)

check_all: check_ftcs check_crankn check_dufrank
