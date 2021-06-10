# ./heat alpha=8.2e-10 lenx=0.25 dx=0.01 dt=100 maxt=5580000 outi=100 savi=1000 bc0=233.15 bc1=294.261 ic="const(294.261)"
ERRBND ?= 1e-6
PTOOL ?= visit
RUNAME ?= heat_results
PIPEWIDTH ?= 0.1
RM = rm

CXXFLAGS = --std=c++14
