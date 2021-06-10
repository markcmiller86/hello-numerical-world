# edit environment variables in make.inc
include make.inc

HDR = Number.H Half.H
SRC = heat.C utils.C args.C exact.C ftcs.C crankn.C dufrank.C
OBJ = $(SRC:.C=.o)
GCOV = $(SRC:.C=.C.gcov) $(SRC:.C=.gcda) $(SRC:.C=.gcno) $(HDR:.H=.H.gcov)
EXE = heat

# Implicit rule for object files
%.o : %.C
	$(CXX) -c $(CSTD) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# Help is default target
help:
	@echo "Targets:"
	@echo "    heat: makes the default heat application (double precision)"
	@echo "    heat-half: makes the heat application with half precision" 
	@echo "    heat-single: makes the heat application with single precision" 
	@echo "    heat-double: makes the heat application with double precision" 
	@echo "    heat-long-double: makes the heat application with long-double precision" 
	@echo "    PTOOL=[gnuplot,matplotlib,visit] RUNAME=<run-dir-name> plot: plots results"
	@echo "    check: runs various tests confirming steady-state is linear"


# Linking the final heat app
heat: $(OBJ)
	$(CXX) -o heat $(OBJ) $(LDFLAGS) -lm

# All objects depend on header
$(OBJ): $(HDR)

# Convenience variable/target for half-precision
heat-half : CPPFLAGS=-DFPTYPE=0
heat-half : $(OBJ)
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
	$(RM) -rf heat heat-half heat-single heat-double heat-long-double

clean: check_clean
	$(RM) -f $(OBJ) $(EXE) $(GCOV)

#
# Run for a long time with random initial condition
# and confirm linear steady-state upon termination
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
