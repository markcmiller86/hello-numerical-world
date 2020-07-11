# ./heat alpha=8.2e-10 lenx=0.25 dx=0.01 dt=100 maxt=5580000 outi=100 savi=1000 bc0=233.15 bc1=294.261 ic="const(294.261)"
PTOOL ?= visit
RUNAME ?= heat_results
RM = rm

SRC = heat.C utils.C args.C exact.C ftcs.C upwind15.C crankn.C
OBJ = $(SRC:.C=.o)
EXE = heat

# Implicit rule for object files
%.o : %.C
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

# Linking the final heat app
heat: $(OBJ)
	$(CXX) -o heat $(OBJ) $(LDFLAGS) -lm

clean:
	$(RM) $(OBJ) $(EXE)
	$(RM) -rf check

plot:
	@./tools/run_$(PTOOL).sh $(RUNAME)

#
# Run for a long time with random initial condition
# and confirm linear stead-state upon termination
#
check/check_soln_final.curve:
	./heat runame=check outi=0 maxt=-0.5e-7 ic="rand(0,0.2,2)"

check: heat check/check_soln_final.curve
	cat check/check_soln_final.curve
	./check.sh check/check_soln_final.curve 0
