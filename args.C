#include "heat.H"

static char clargs[2048];

#define HANDLE_ARG(VAR, TYPE, STYLE, HELP) \
{ \
    char const *style = #STYLE; \
    char const *q = style[1]=='s'?"\"":""; \
    void *valp = (void*) &VAR; \
    int const len = strlen(#VAR)+1; \
    std::stringstream strmvar; \
    for (i = 1; i < argc; i++) \
    {\
        int valid_style = style[1]=='d'||style[1]=='g'||style[1]=='s'; \
        if (strncmp(argv[i], #VAR"=", len)) \
            continue; \
        assert(valid_style); \
	if (strlen(argv[i]+len)) \
        {\
            if      (style[1] == 'd') /* int */ \
                *((int*) valp) = (int) strtol(argv[i]+len,0,10); \
            else if (style[1] == 'g') /* double */ \
                *((Number*) valp) = (fpnumber) strtod(argv[i]+len,0); \
            else if (style[1] == 's') /* char* */ \
                *((char**) valp) = (char*) strdup(argv[i]+len); \
        }\
    }\
    strmvar << VAR; \
    if (help) \
    {\
        char tmp[256]; \
        int len = snprintf(tmp, sizeof(tmp), "    %s=%s%s%s", \
            #VAR, q, strmvar.str().c_str(), q);\
        snprintf(tmp, sizeof(tmp), "%s (%s)", #HELP, #TYPE); \
        fprintf(stderr, "    %s=%s%s%s%*s\n", \
            #VAR, q, strmvar.str().c_str(), q, 80-len, tmp);\
    }\
    else \
    { \
        char tmp[64]; \
        fprintf(stderr, "    %s=%s%s%s\n", \
            #VAR, q, strmvar.str().c_str(), q);\
        snprintf(tmp, sizeof(tmp), "    %s=%s%s%s\n", \
            #VAR, q, strmvar.str().c_str(), q);\
        strcat(clargs, tmp); \
    } \
}

extern Number alpha;
extern Number lenx;
extern Number dx;
extern Number dt;
extern Number maxt;
extern Number bc0;
extern Number bc1;
extern Number min_change;
extern char const *runame;
extern char const *ic;
extern char const *alg;
extern int savi;
extern int save;
extern int outi;
extern int noout;
extern int nt;
int const prec = FPTYPE;

static void handle_help(char const *argv0)
{
    fprintf(stderr, "Examples...\n");
    fprintf(stderr, "    %s dx=0.01 dt=0.0002 alg=ftcs\n", argv0);
    fprintf(stderr, "    %s dx=0.1 bc0=273 bc1=273 ic=\"spikes(273,5,373)\"\n", argv0);
    fprintf(stderr, "    %s ic=\"help\" to get help on initial condition options\n", argv0);
}

static void handle_ic_help()
{
    fprintf(stderr, "Initial condition options...\n");
    fprintf(stderr, "    ic=\"const(V)\" constant value, V\n");
    fprintf(stderr, "    ic=\"ramp(L,R)\" linear ramp with value L @ x=0 and R @ x=lenx\n");
    fprintf(stderr, "    ic=\"step(L,Mx,R)\" a step function with value L for x<Mx and R for x>=Mx\n");
    fprintf(stderr, "    ic=\"rand(S,B,A)\" random values in the range [B-A,B+A] using seed S\n");
    fprintf(stderr, "    ic=\"sin(A,w)\" a sin wave with amplitude A and frequency w\n");
    fprintf(stderr, "    ic=\"spikes(C,A0,X0,A1,X1,...)\" a constant value, C with N spikes of amplitude Ai at position Xi\n");
    fprintf(stderr, "    ic=\"file(foo.dat)\" : read initial condition data from the file foo.dat\n");
    fprintf(stderr,
        "Be sure to use double-quotes (\") as shown and you may also need to set boundary"
        "\nconditions such that they *combine* smoothly with the initial conditions.\n");
}

void
process_args(int argc, char **argv)
{
    int i;
    int help = 0;

    // quick pass for 'help' anywhere on command line
    clargs[0] ='\0';
    for (i = 0; i < argc && !help; i++)
        help = 0!=strcasestr(argv[i], "help");
    
    if (help)
        fprintf(stderr, "Usage: %s <arg>=<value> <arg>=<value>...\n", argv[0]);

    HANDLE_ARG(alpha, fpnumber, %g, material thermal diffusivity (sq-meters/second));
    HANDLE_ARG(lenx, fpnumber, %g, material length (meters));
    HANDLE_ARG(dx, fpnumber, %g, x-incriment. Best if lenx/dx==int. (meters));
    HANDLE_ARG(dt, fpnumber, %g, t-incriment (seconds));
    HANDLE_ARG(maxt, fpnumber, %g, >0:max sim time (seconds) | <0:min l2 change in soln);
    HANDLE_ARG(bc0, fpnumber, %g, boundary condition @ x=0: u(0,t) (Kelvin));
    HANDLE_ARG(bc1, fpnumber, %g, boundary condition @ x=lenx: u(lenx,t) (Kelvin));
    HANDLE_ARG(runame, char*, %s, name to give run and results dir);
    HANDLE_ARG(ic, char*, %s, initial condition @ t=0: u(x,0) (Kelvin));
    HANDLE_ARG(alg, char*, %s, algorithm ftcs|dufrank|crankn);
#ifdef _OPENMP
    HANDLE_ARG(nt, int, %d, number of parallel tasks);
#else
    HANDLE_ARG(nt, int, %d, parallel tasking is DISABLED!!);
    nt = 0;
#endif
    HANDLE_ARG(savi, int, %d, save every i-th solution step);
    HANDLE_ARG(save, int, %d, save error in every saved solution);
    HANDLE_ARG(outi, int, %d, output progress every i-th solution step);
    HANDLE_ARG(noout, int, %d, disable all file outputs);
    HANDLE_ARG(prec, const int, %d, precision 0=half/1=float/2=double/3=long double)

    if (help)
        handle_help(argv[0]);

    if (strcasestr(ic, "help"))
    {
        handle_ic_help();
        exit(1);
    }

    if (help)
        exit(1);

    // Handle possible invalid combination of parallel tasking and algorithm
#ifdef _OPENMP
    if (nt > 1 && !strcmp(alg,"crankn"))
    {
        fprintf(stderr, "The \"crankn\" algorithm does not yet support parallel\n");
        exit(1);
    } 
#endif 

    // Handle possible termination by change threshold criterion
    if (maxt < 0)
    {
        min_change = -maxt * -maxt;
        maxt = INT_MAX;
    }

    // Handle output results dir creation and save of command-line
    if (access(runame, F_OK) == 0)
    {
        fprintf(stderr, "An entry \"%s\" already exists\n", runame);
        exit(1);
    } 

    // Make the output dir and save clargs there too
    mkdir(runame, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    char fname[128];
    snprintf(fname,sizeof(fname), "%s/clargs.out", runame);
    FILE *outf = fopen(fname, "w");
    fprintf(outf, "%s", clargs);
    fclose(outf);
}
