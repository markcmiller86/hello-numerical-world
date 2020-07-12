# ./heat alpha=8.2e-10 lenx=0.25 dx=0.01 dt=100 maxt=5580000 outi=100 savi=1000 bc0=233.15 bc1=294.261 ic="const(294.261)"
PTOOL ?= visit
RUNAME ?= heat_results
RM = rm

HDR = Double.H
SRC = heat.C utils.C args.C exact.C ftcs.C upwind15.C crankn.C
OBJ = $(SRC:.C=.o)
GCOV = $(SRC:.C=.C.gcov) $(SRC:.C=.gcda) $(SRC:.C=.gcno) $(HDR:.H=.H.gcov)
EXE = heat

# Implicit rule for object files
%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# Linking the final heat app
heat: $(OBJ)
	$(CXX) -o heat $(OBJ) $(LDFLAGS) -lm

check_clean:
	$(RM) -rf check check_crankn check_upwind15

clean: check_clean
	$(RM) -f $(OBJ) $(EXE) $(GCOV)

plot:
	@./tools/run_$(PTOOL).sh $(RUNAME)

#
# Run for a long time with random initial condition
# and confirm linear stead-state upon termination
#
check/check_soln_final.curve:
	./heat runame=check outi=0 maxt=-5e-8 ic="rand(0,0.2,2)"

check: heat check/check_soln_final.curve
	cat check/check_soln_final.curve
	./check.sh check/check_soln_final.curve 0

check_ftcs: check

check_crankn/check_crankn_soln_final.curve:
	./heat alg=crankn runame=check_crankn outi=0 maxt=-5e-8 ic="rand(0,0.2,2)"

check_crankn: heat check_crankn/check_crankn_soln_final.curve
	cat check_crankn/check_crankn_soln_final.curve
	./check.sh check_crankn/check_crankn_soln_final.curve

check_upwind15/check_upwind15_soln_final.curve:
	./heat alg=upwind15 runame=check_upwind15 outi=0 maxt=40 ic="rand(0,0.2,2)"

check_upwind15: heat check_upwind15/check_upwind15_soln_final.curve
	cat check_upwind15/check_upwind15_soln_final.curve
	./check.sh check_upwind15/check_upwind15_soln_final.curve

check_all: check_ftcs check_crankn check_upwind15
