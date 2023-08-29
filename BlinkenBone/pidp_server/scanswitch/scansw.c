/*
 * Scan switches for PiDP-11 front panel
 *
 * Rewritten for libgpiod based upon Oscar's original (v10181230)
 */

#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <gpiod.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define _countof(x) (sizeof x / sizeof x[0])

#define GPIO_NUM    0
#define SETTLE_TIME 1       // time to allow switches to settle (us)
#define LEDROW_FLAGS 0      // no pull
#define COL_FLAGS GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP

unsigned ledrows[] = { 20, 21, 22, 23, 24, 25 };                // LED rows
unsigned rows[] = { 16, 17, 18 };                               // switch rows
unsigned cols[] = { 26, 27, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 }; // columns

int
main(int argc, char **argv)
{
    char *argv0 = (argc > 0) ? argv[0] : "";
    struct gpiod_chip *chip = NULL;
    struct gpiod_line_bulk bulk_ledrows = GPIOD_LINE_BULK_INITIALIZER;
    struct gpiod_line_bulk bulk_rows = GPIOD_LINE_BULK_INITIALIZER;
    struct gpiod_line_bulk bulk_cols = GPIOD_LINE_BULK_INITIALIZER;
    static int row_vals[_countof(rows)] = { 1, 1, 1 };
    static int col_vals[_countof(rows)][_countof(cols)];
    int exit_status = 1;
    unsigned swreg;
    char *cp;
    int row, bit, *ip;

    // determine our name for reserving the lines from the driver
    if ((cp = strrchr(argv0, '/')) != NULL)
        argv0 = cp;

    // open the chip
    if ((chip = gpiod_chip_open_by_number(GPIO_NUM)) == NULL) {
        perror("gpiod_chip_open_by_number");
        goto out;
    }

    // configure the LED rows as inputs with no pull (inert)
    if (gpiod_chip_get_lines(chip, ledrows, _countof(ledrows), &bulk_ledrows) < 0) {
        perror("gpiod_chip_get_lines(ledrows)");
        goto out;
    }
    if (gpiod_line_request_bulk_input_flags(&bulk_ledrows, argv0, LEDROW_FLAGS)) {
        perror("gpiod_line_request_bulk_input_flags(bulk_ledrows)");
        goto out;
    }

    // configure the switch rows as outputs that are all initially high
    if (gpiod_chip_get_lines(chip, rows, _countof(rows), &bulk_rows) < 0) {
        perror("gpiod_chip_get_lines(ledrows)");
        goto out;
    }
    if (gpiod_line_request_bulk_output(&bulk_rows, argv0, row_vals)) {
        perror("gpiod_line_request_bulk_output(bulk_rows)");
        goto out;
    }

    // configure the columns as inputs with pull up
    if (gpiod_chip_get_lines(chip, cols, _countof(cols), &bulk_cols) < 0) {
        perror("gpiod_chip_get_lines(ledrows)");
        goto out;
    }
    if (gpiod_line_request_bulk_input_flags(&bulk_cols, argv0, COL_FLAGS) < 0) {
        perror("gpiod_line_request_bulk_input_flags(bulk_cols)");
        goto out;
    }

    // for each row:
    //  drive that row low (and the others high)
    //  wait briefly to let things settle
    //  read the columns
    for (row = 0; row < _countof(rows); row++) {
        row_vals[row] = 0;
        if (gpiod_line_set_value_bulk(&bulk_rows, row_vals) < 0) {
            perror("gpiod_line_set_value");
            goto out;
        }
        usleep(SETTLE_TIME);
        if (gpiod_line_get_value_bulk(&bulk_cols, col_vals[row]) < 0) {
            perror("gpiod_line_get_value_bulk");
            goto out;
        }
        row_vals[row] = 1;
    }

    // construct the final value from the first 22 bits
    // bits are in little-endian order and are inverted
    // we rely upon the order of elements in 2-d C arrays
    swreg = 0;
    for (bit = 0, ip = col_vals[0]; bit < 22; bit++, ip++)
        swreg |= (!*ip) << bit;
    printf("%d\n", swreg);

    exit_status = 0;

out:
    // clean up and exit
    if (chip != NULL)
        gpiod_chip_close(chip);
    return exit_status;
}