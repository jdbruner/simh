/*
 * simtw - simh tape write
 *
 * This is primarily intended for wrapping a single file (e.g., tar archive)
 * for SIMH, but it can concatenate multiple files if desired. However, all
 * files have the same block size; hence, this isn't suitable for mixed-usage,
 * e.g., bootstrap tapes.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <strings.h>
#include <assert.h>

typedef enum bool { false, true } bool;

static const int MAX_BLOCK_SIZE = 20;

static bool wrapfile(FILE *ifp, FILE *ofp, int blocksize, bool verbose);

int
main(int argc, char **argv)
{
    int blocksize = MAX_BLOCK_SIZE;
    bool verbose = false;
    FILE *ofp = stdout;
    register FILE *ifp = stdin;
    register int option;
    register int i;
    static const unsigned char end_of_medium[4] = { 0xff, 0xff, 0xff, 0xff };

    opterr = 0;
    while ((option = getopt(argc, argv, "b:o:hv")) >= 0) {
        switch (option) {
        case 'b':
            blocksize = atoi(optarg);
            if (blocksize < 1 || blocksize > MAX_BLOCK_SIZE) {
                fprintf(stderr, "block size (%d) must be between 1 and %d\n",
                    blocksize, MAX_BLOCK_SIZE);
                return EXIT_FAILURE;
            }
            break;

       case 'o':
            if ((ofp = fopen(optarg, "w")) == NULL) {
                perror(optarg);
                return EXIT_FAILURE;
            }
            break;

        case 'v':
            verbose = true;
            break;

        default:
            fprintf(stderr, "Unknown argument: \"%s\"\n", argv[optind-1]);
            /* fall through */
        case 'h':
            fprintf(stderr, "Syntax: %s [-v] [-b blocking] [-o outfile] [infile ...]\n", argv[0]);
            return (option == 'h') ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    }

    if (optind == argc) {
        if (verbose)
            fprintf(stderr, "Processing standard input:\n");
        if (!wrapfile(ifp, ofp, blocksize, verbose))
            return EXIT_FAILURE;
    } else {
        for (i = optind; i < argc; i++) {
            if ((ifp = fopen(argv[i], "r")) == NULL) {
                perror(argv[i]);
                return EXIT_FAILURE;
            }
            if (verbose)
                fprintf(stderr, "Processing \"%s\"\n", argv[i]);
            if (!wrapfile(ifp, ofp, blocksize, verbose))
                return EXIT_FAILURE;
            fclose(ifp);
        }
    }

    if (fwrite(end_of_medium, 1, sizeof end_of_medium, ofp) != sizeof end_of_medium) {
        perror("fwrite");
        return EXIT_FAILURE;
    }
    if (verbose)
        fprintf(stderr, "    0xFFFFFFFF\n");

    fflush(ofp);
    return EXIT_SUCCESS;
}

static bool
wrapfile(FILE *ifp, FILE *ofp, int blocksize, bool verbose)
{
    register size_t n;
    size_t bufsize = blocksize * 512;
    unsigned char buf[MAX_BLOCK_SIZE * 512];
    unsigned char header_buf[4];
    static const unsigned char filemark_buf[4];

    assert(bufsize <= sizeof buf);

    header_buf[0] = bufsize & 0xff;
    header_buf[1] = (bufsize >> 8) & 0xff;
    header_buf[2] = (bufsize >> 16) & 0xff;
    header_buf[3] = (bufsize >> 24) & 0xff;

    while ((n = fread(buf, 1, bufsize, ifp)) > 0) {
        if (n < bufsize)
            bzero(buf+n, bufsize-n);
        if (fwrite(header_buf, 1, sizeof header_buf, ofp) != sizeof header_buf ||
            fwrite(buf, 1, bufsize, ofp) != bufsize ||
            fwrite(header_buf, 1, sizeof header_buf, ofp) != sizeof header_buf)
            break;
        if (verbose) {
            if (n < bufsize)
                fprintf(stderr, "    0x%08lX (%lu) [actual %lu]\n", bufsize, bufsize, n);
            else
                fprintf(stderr, "    0x%08lX (%lu)\n", bufsize, bufsize);
        }
    }

    if (ferror(ifp)) {
        perror("fread");
        return false;
    }
    if (ferror(ofp)) {
        perror("fwrite");
        return false;
    }

    if (fwrite(filemark_buf, 1, sizeof filemark_buf, ofp) != sizeof filemark_buf) {
        perror("fwrite");
        return false;
    }
    if (verbose)
        fprintf(stderr, "    0x00000000\n");

    return true;
}
