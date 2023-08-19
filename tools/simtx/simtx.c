/*
 * simtx - simh tape extract
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

typedef enum bool { false, true } bool;

static const unsigned long SIMH_TP_FILEMARK = 0UL;
static const unsigned long SIMH_TP_ENDOFMEDIUM = 0xffffffffUL;
static const unsigned long SIMH_TP_ERASEGAP = 0xfffffffeUL;

static void print_syntax(const char *const argv0);
static bool extract(FILE *const ifp, FILE *const ofp, unsigned long *const hdrp, const bool verbose);

int
main(int argc, char **argv)
{
    char *infile = NULL, *outfile = NULL;
    FILE *ifp = stdin, *ofp = stdout;
    unsigned long header;
    register int skipcount = 0;
    register bool verbose = false;
    register int option;

    opterr = 0;
    while ((option = getopt(argc, argv, "s:i:o:hv")) >= 0) {
        switch (option) {
        case 's':
            skipcount = atoi(optarg);
            if (skipcount < 0) {
                fprintf(stderr, "skipcount (%s) must be positive\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'i':
            infile = optarg;
            break;
        case 'o':
            outfile = optarg;
            break;
        case 'h':
            print_syntax(*argv);
            return EXIT_SUCCESS;
        case 'v':
            verbose = true;
            break;
        default:
            fprintf(stderr, "Unknown argument: \"%s\"\n", argv[optind-1]);
            print_syntax(*argv);
            return EXIT_FAILURE;
        }
    }
    if (optind < argc) {
        /* extra unknown arguments */
        print_syntax(*argv);
        return EXIT_FAILURE;
    }

    if (infile != NULL && (ifp = fopen(infile, "r")) == NULL) {
        perror(infile);
        return EXIT_FAILURE;
    }

    if (outfile != NULL && (ofp = fopen(outfile, "w")) == NULL) {
        perror("outfile");
        return EXIT_FAILURE;
    }

    while (skipcount-- > 0) {
        if (!extract(ifp, NULL, &header, verbose) || header != SIMH_TP_FILEMARK) {
            fprintf(stderr, "skip failed (reached end of file)\n");
            return EXIT_FAILURE;
        }
    }
    if (!extract(ifp, ofp, NULL, verbose))
        return EXIT_FAILURE;

    fflush(ofp);
    return EXIT_SUCCESS;
}

static void
print_syntax(const char *const argv0)
{
    fprintf(stderr, "Syntax: %s [-v] [-s skipcount] [-i infile] [-o outfile]\n", argv0);
}

/*
 * extract a file from a SIMH tape
 * ifp - input FILE
 * ofp - output FILE or NULL
 * hdrp - if non-NULL, a location to which the terminating header is returned
 * verbose - trace activity (on standard error)
 * returns true for success and false for error (e.g., premature EOF)
 */
static bool
extract(FILE *const ifp, FILE *const ofp, unsigned long *const hdrp, const bool verbose)
{
    unsigned long header, trailer;
    register long nrec = 0;
    register long remaining;
    register size_t bufsize;
    unsigned char buf[20 * 512];    /* default TAR block size */

    for (nrec = 0; true; nrec++) {
        if (fread(buf, 1, 4, ifp) != 4) {
            if (feof(ifp)) {
                /* EOF - allow this, treat as an end-of-medium indicaton */
                header = SIMH_TP_ENDOFMEDIUM;
            } else {
                if (ferror(ifp))
                    perror("fread");
                return false;
            }
        } else
            header = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);

        if (verbose)
            fprintf(stderr, "0x%08lX (%lu)\n", header, header);

        if (header == SIMH_TP_FILEMARK || header == SIMH_TP_ENDOFMEDIUM ||
            header == SIMH_TP_ERASEGAP) 
            break;

        for (remaining = (long)header; remaining > 0; remaining -= bufsize) {
            bufsize = (remaining < sizeof buf) ? remaining : sizeof buf;
            if (fread(buf, 1, bufsize, ifp) != bufsize) {
                if (feof(ifp))
                    fprintf(stderr, "premature EOF on input\n");
                else if (ferror(ifp))
                    perror("fread");
                return false;
            }
            if (ofp != NULL && fwrite(buf, 1, bufsize, ofp) != bufsize) {
                perror("fwrite");
                return false;
            }
        }

        if (fread(buf, 1, 4, ifp) != 4) {
            if (feof(ifp))
                fprintf(stderr, "premature EOF on input\n");
            else if (ferror(ifp))
                perror("fread");
            return false;
        }
        trailer = buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
        if (header != trailer) {
            fprintf(stderr, "header and trailer words do not match (0x%08lX != 0x%08lX)",
                header, trailer);
            return false;
        }
    }

    if (verbose)
        fprintf(stderr, "%ld records transferred\n", nrec);

    if (hdrp != NULL)
        *hdrp = header;
    return true;
}