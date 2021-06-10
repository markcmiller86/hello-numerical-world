#include "heat.H"

// Utilities
Number
l2_norm(int n, Number const *a, Number const *b)
{
    int i;
    Number sum(0.0);
    for (i = 0; i < n; i++)
    {
        Number diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum / castNum(n);
}

void
copy(Vector &dst, const Vector &src)
{
    assert(dst.size() == src.size());
    int i;
    for (i = 0; i < dst.size(); i++)
        dst[i] = src[i];
}

void write_array(ArrType t, Args &arg, Number step, const Vector &a, int time)
{
    int i;
    int n = a.size();
    char fname[128];
    char vname[64];
    FILE *outf;

    if (arg.noout) return;

    switch(t) {
    case ArrType::TSTART:
        snprintf(fname, sizeof(fname), "%s/%s_soln_00000.curve", arg.runame, arg.runame);
        snprintf(vname, sizeof(vname), "Temperature");
        break;
    case ArrType::TFINAL:
        snprintf(fname, sizeof(fname), "%s/%s_soln_final.curve", arg.runame, arg.runame);
        snprintf(vname, sizeof(vname), "Temperature");
        break;
    case ArrType::RESIDUAL:
        snprintf(fname, sizeof(fname), "%s/%s_change.curve", arg.runame, arg.runame);
        snprintf(vname, sizeof(vname), "%s/%s_l2_change", arg.runame, arg.runame);
        n = time < n ? time : n;
        break;
    case ArrType::ERROR:
        snprintf(fname, sizeof(fname), "%s/%s_error.curve", arg.runame, arg.runame);
        snprintf(vname, sizeof(vname), "%s/%s_l2", arg.runame, arg.runame);
        n = time < n ? time : n;
        break;
    case ArrType::EXACT:
        snprintf(fname, sizeof(fname), "%s/%s_exact_%05d.curve", arg.runame, arg.runame, time);
        snprintf(vname, sizeof(vname), "exact_temperature");
        break;
    case ArrType::STEP:
        snprintf(fname, sizeof(fname), "%s/%s_soln_%05d.curve", arg.runame, arg.runame, time);
        snprintf(vname, sizeof(vname), "Temperature");
        break;
    }


    outf = fopen(fname,"w");
    fprintf(outf, "# %s\n", vname);
    for (i = 0; i < n; i++)
    {
#if FPTYPE == 0
        fprintf(outf, "%- 7.5e %- 7.5e\n", double(i*step), double(a[i]));
#elif FPTYPE == 1
        fprintf(outf, "%- 11.9e %- 11.9e\n", (double) (i*step), (double) a[i]);
#elif FPTYPE == 2
        fprintf(outf, "%- 19.17e %- 19.17e\n", (double) (i*step), (double) a[i]);
#elif FPTYPE == 3
        fprintf(outf, "%- 27.25Le %- 27.25Le\n", (Number) (i*step), (Number) a[i]);
#elif 
#error UNKNOWN FPTYPE
#endif
    }
    fclose(outf);
}

void set_initial_condition(Args &arg, Vector &a)
{
    int n = a.size();
    int i;
    double x;

    if (!strncmp(arg.ic, "const(", 6)) /* const(val) */
    {
        double cval = strtod(arg.ic+6, 0);
        for (i = 0; i < n; i++)
            a[i] = cval;
    }
    else if (!strncmp(arg.ic, "step(", 5)) /* step(left,xmid,right) */
    {
        char *p;
        double left = strtod(arg.ic+5, &p);
        double xmid = strtod(p+1, &p);
        double right = strtod(p+1, 0);
        for (i = 0, x = 0; i < n; i++, x+=arg.dx)
        {
            if (x < xmid) a[i] = left;
            else          a[i] = right;
        }
    }
    else if (!strncmp(arg.ic, "ramp(", 5)) /* ramp(left,right) */
    {
        char *p;
        double left = strtod(arg.ic+5, &p);
        double right = strtod(p+1, 0);
        double dv = (right-left)/(n-1);
        for (i = 0, x = left; i < n; i++, x+=dv)
            a[i] = x;
    }
    else if (!strncmp(arg.ic, "rand(", 5)) /* rand(seed,base,amp) */
    {
        char *p, *ep;
        int seed = (int) strtol(arg.ic+5,&p,10);
        double base = strtod(p+1, &p);
        double amp = strtod(p+1, 0);
        const double maxr = ((long long)1<<31)-1;
        srandom(seed);
        for (i = 0; i < n; i++)
            a[i] = base + amp * (2*random()/maxr - 1);
    }
    else if (!strncmp(arg.ic, "sin(", 4)) /* A*sin(PI*w*x) */
    {
        char *p;
        double amp = strtod(arg.ic+4,&p);
        double w = strtod(p+1, 0);
        for (i = 0, x = 0; i < n; i++, x+=arg.dx)
            a[i] = amp * sin(M_PI*w*x);
    }
    else if (!strncmp(arg.ic, "spikes(", 7)) /* spikes(Const,Amp,Loc,Amp,Loc,...) */
    {
        char *next;
        double cval = strtod(arg.ic+7, &next);
        char const *p = next;
        for (i = 0, x = 0; i < n; i++)
            a[i] = cval;
        while (*p != ')')
        {
            char *ep_amp, *ep_idx;
            double amp = strtod(p+1, &ep_amp);
            int idx = (int) strtod(ep_amp+1, &ep_idx);
            assert(idx<n);
            a[idx] = amp;
            p = ep_idx;
        }
    }
    else if (!strncmp(arg.ic, "file(", 5)) /* file(numbers.dat) */
    {
        FILE *icfile;
        char *filename = strdup(&arg.ic[5]);
        char *parenchar  = strchr(filename, ')');
        double val;

        /* open the file */
        assert(parenchar);
        *parenchar = '\0';
        icfile = fopen(filename, "r");
        assert(icfile);

        /* read the data */
        for (i = 0; (fscanf(icfile, "%lg", &val) == 1) && (i<n); i++)
            a[i] = val;

        /* sanity checks */
        assert(!ferror(icfile));
        assert(feof(icfile));
        assert(i==n);

        /* cleanup */
        fclose(icfile);
        free(filename);
    }
    write_array(ArrType::TSTART, arg, arg.dx, a);
}
