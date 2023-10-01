#ifndef _PARAMS_H_
#define _PARAMS_H_

#include "common.h"

typedef struct Params {
    unsigned int  m_size;
    unsigned int  n_size;
    unsigned int  n_warmup;
    unsigned int  n_reps;
    const char *         file_name;    
}Params;

static void usage() {
    fprintf(stderr,
            "\nUsage:  ./program [options]"
            "\n"
            "\nGeneral options:"
            "\n    -h        help"
            "\n    -w <W>    # of untimed warmup iterations (default=1)"
            "\n    -e <E>    # of timed repetition iterations (default=3)"
            "\n     -f Filename  " 
            "\n"
            "\nBenchmark-specific options:"
            "\n");
}

struct Params input_params(int argc, char **argv) {
    struct Params p;
    p.n_warmup      = 1;
    p.n_reps        = 3;
    p.file_name    = "/home/saisan/dummy.csv";

    int opt;
    while((opt = getopt(argc, argv, "f:hw:e:")) >= 0) {
        switch(opt) {
            case 'h':
                usage();
                exit(0);
                break;
            case 'f': p.file_name    = optarg;       break;
            case 'w': p.n_warmup      = atoi(optarg); break;
            case 'e': p.n_reps        = atoi(optarg); break;
            default:
                      fprintf(stderr, "\nUnrecognized option!\n");
                      usage();
                      exit(0);
        }
    }
    assert(NR_DPUS > 0 && "Invalid # of dpus!");

    return p;
}
#endif
