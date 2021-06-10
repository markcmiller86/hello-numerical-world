#pragma once

// Command-line argument variables
struct Args {
    // Number of points in space, x, and time, t.
    int Nx;
    int Nt;

    int noout;
    int savi;
    int outi;
    int save;
    char const *runame;
    char const *alg;
    char const *ic;
    Number lenx;
    Number alpha;
    Number dt;
    Number dx;
    Number bc0;
    Number bc1;
    Number maxt;
    Number min_change;

    Args() :
        Nx(0), Nt(0),
        noout        ( 0 ),
        savi         ( 0 ),
        outi         ( 100 ),
        save         ( 0 ),
        runame ( "heat_results" ),
        alg  ( "ftcs" ),
        ic   ( "const(1)" ),
        lenx      ( Number(1.0) ),
        alpha     ( Number(0.2) ),
        dt        ( Number(0.004) ),
        dx        ( Number(0.1) ),
        bc0       ( Number(0.0) ),
        bc1       ( Number(1.0) ),
        maxt      ( Number(2.0) ),
        min_change ( Number(1e-8*1e-8) ) {}
};

Args process_args(int argc, char **argv);
