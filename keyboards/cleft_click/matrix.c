#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "config.h"
#include  "timer.h"

#ifndef DEBOUNCE
#   define DEBOUNCE	5
#endif
#ifndef SHIFT_DELAY
#   define SHIFT_DELAY 5
#endif

#if (MATRIX_COLS <= 8)
#    define print_matrix_header()  print("\nr/c 01234567\n")
#    define print_matrix_row(row)  print_bin_reverse8(matrix_get_row(row))
#    define matrix_bitpop(i)       bitpop(matrix[i])
#    define ROW_SHIFTER ((uint8_t)1)
#elif (MATRIX_COLS <= 16)
#    define print_matrix_header()  print("\nr/c 0123456789ABCDEF\n")
#    define print_matrix_row(row)  print_bin_reverse16(matrix_get_row(row))
#    define matrix_bitpop(i)       bitpop16(matrix[i])
#    define ROW_SHIFTER ((uint16_t)1)
#elif (MATRIX_COLS <= 32)
#    define print_matrix_header()  print("\nr/c 0123456789ABCDEF0123456789ABCDEF\n")
#    define print_matrix_row(row)  print_bin_reverse32(matrix_get_row(row))
#    define matrix_bitpop(i)       bitpop32(matrix[i])
#    define ROW_SHIFTER  ((uint32_t)1)
#endif

static bool debouncing = false;
static uint16_t debouncing_time;

enum pin_mode {
	OUTPUT = 0,
	INPUT = 1,
	INPUT_PULLUP = 2
};
typedef enum pin_mode pin_mode_t;

#define HIGH true
#define LOW false

static const uint8_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const uint8_t col_pins[RIGHT_COLS] = MATRIX_COL_PINS;

/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static void init_rows(void);
static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t col);
static void unselect_cols(void);
static void select_col(uint8_t col);
static void unselect_col(uint8_t col);

// 74HC165 (shift in register) helpers
static matrix_row_t shift_in_byte(void);
static void shift_load(void);
static matrix_row_t shift_in(void);
static void shift_in_clk(void);
static matrix_row_t shift_read(void);

// 74HC164 (shift out register) helpers
static void shift_out_clk(void);
static void shift_data(bool data);
static void shift_out_byte(uint8_t data);

static void digitalWrite(uint8_t pin, bool val);
static bool digitalRead(uint8_t pin);
static void pinMode(uint8_t pin, pin_mode_t mode);

#ifdef DEBUG_MATRIX_SCAN_RATE
uint32_t matrix_timer;
uint32_t matrix_scan_count;
#endif

__attribute__ ((weak))
void matrix_init_user(void) {}

__attribute__ ((weak))
void matrix_scan_user(void) {}

__attribute__ ((weak))
void matrix_init_kb(void) {
  matrix_init_user();
}

__attribute__ ((weak))
void matrix_scan_kb(void) {
  matrix_scan_user();
}

inline
uint8_t matrix_rows(void) {
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    // To use PORTF disable JTAG with writing JTD bit twice within four cycles.
    #if  (defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__) || defined(__AVR_ATmega32U4__))
        MCUCR |= _BV(JTD);
        MCUCR |= _BV(JTD);
    #endif

    unselect_cols();
    init_rows();

    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
			matrix[i] = 0;
			matrix_debouncing[i] = 0;
		}

		pinMode(SHIFT_OUT_CLK, OUTPUT);
		pinMode(SHIFT_OUT_DATA, OUTPUT);
		pinMode(SHIFT_IN_CLK, OUTPUT);
		pinMode(SHIFT_IN_DATA, INPUT_PULLUP);
		pinMode(SHIFT_IN_LOAD, OUTPUT);

    matrix_init_quantum();
}

uint8_t matrix_scan(void)
{
    for (uint8_t i = 0; i < MATRIX_COLS; i++) {
        bool matrix_changed = read_rows_on_col(matrix_debouncing, i);
        if (matrix_changed) {
					if (debouncing) {
							debug("bounce!: "); debug_hex(debouncing_time); debug("\n");
					}
					debouncing = true;
					debouncing_time = timer_read();
        }
    }

    if (debouncing && (timer_elapsed(debouncing_time) > DEBOUNCE)) {
				for(uint8_t row = 0; row < MATRIX_ROWS; ++row) {
					matrix[row] = matrix_debouncing[row];
				}
				debouncing = false;
    }

    matrix_scan_quantum();
    return 1;
}

bool matrix_is_modified(void)
{
    if (debouncing) return false;
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)ROW_SHIFTER<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print_matrix_header();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        print_matrix_row(row);
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

static void init_rows() {
	for (int i = 0; i < MATRIX_ROWS; ++i)
		pinMode(row_pins[i], INPUT_PULLUP);
}

static bool read_rows_on_col(matrix_row_t current_matrix[], uint8_t col)
{
	bool matrix_changed = false;

	select_col(col);
	wait_us(30);

	if (col < LEFT_COLS) {
		matrix_row_t ret = shift_in_byte();
		ret >>= 8 - MATRIX_ROWS;

		for(int i = 0; i < MATRIX_ROWS; ++i) {
			matrix_row_t last_row_value = current_matrix[i];
			if((ret & (ROW_SHIFTER << i)) == 0)
				current_matrix[i] |= ROW_SHIFTER << col;
			else
				current_matrix[i] &= ~(ROW_SHIFTER << col);
			if ((last_row_value != current_matrix[i]) && !(matrix_changed))
					matrix_changed = true;
		}
	}
	else {
		for(int i = 0; i < MATRIX_ROWS; ++i) {
			matrix_row_t last_row_value = current_matrix[i];
			if(!digitalRead(row_pins[i]))
				current_matrix[i] |= ROW_SHIFTER << col;
			else
				current_matrix[i] &= ~(ROW_SHIFTER << col);
			if ((last_row_value != current_matrix[i]) && !(matrix_changed))
					matrix_changed = true;
		}
	}

	unselect_col(col);

	return matrix_changed;
}

static void select_col(uint8_t col) {
    if (col < LEFT_COLS) {
			shift_out_byte(1 << col);
    }
		else {
			col -= LEFT_COLS;
			pinMode(col_pins[col], OUTPUT);
			digitalWrite(col_pins[col], LOW);
    }
}

static void unselect_col(uint8_t col) {
  if (col < LEFT_COLS) {
		shift_out_byte(0);
	}
	else {
		col -= LEFT_COLS;
		pinMode(col_pins[col], INPUT_PULLUP);
	}
}

static void unselect_cols(void) {
	for(int i = 0; i < RIGHT_COLS; ++i)
		pinMode(col_pins[i], INPUT_PULLUP);
	shift_out_byte(0);
}


static matrix_row_t shift_in_byte() {
  shift_load();
  uint8_t ret = 0;
  for (int i = 0; i < 8; ++i) {
	    ret |= shift_in() << i;
	  }
  return ret;
}

static void shift_load(void) {
	digitalWrite(SHIFT_IN_LOAD, false);
	shift_in_clk();
	digitalWrite(SHIFT_IN_LOAD, true);
}

static matrix_row_t shift_in(void) {
	matrix_row_t ret = shift_read();
	shift_in_clk();
	return ret;
}

static void shift_in_clk(void) {
		digitalWrite(SHIFT_IN_CLK, true);
		wait_us(SHIFT_DELAY);
		digitalWrite(SHIFT_IN_CLK, false);
}

static matrix_row_t shift_read(void) {
	if(digitalRead(SHIFT_IN_DATA))
		return 1;
	return 0;
}


static void shift_out_clk(void) {
	digitalWrite(SHIFT_OUT_CLK, HIGH);
	wait_us(SHIFT_DELAY);
	digitalWrite(SHIFT_OUT_CLK, LOW);
}

static void shift_data(bool data) {
	digitalWrite(SHIFT_OUT_DATA, !data);
}

static void shift_out_byte(uint8_t data) {
	for(int i = 7; i >= 0; --i) {
		shift_data(data & (1 << i));
		shift_out_clk();
	}
}


static void digitalWrite(uint8_t pin, bool val) {
	if(val)
		_SFR_IO8((pin >> 4) + 2) |=  _BV(pin & 0xF); // HIGH
	else
		_SFR_IO8((pin >> 4) + 2) &= ~_BV(pin & 0xF); // LOW
}

static bool digitalRead(uint8_t pin) {
		return _SFR_IO8((pin >> 4)) & _BV(pin & 0xF);
}

static void pinMode(uint8_t pin, pin_mode_t mode) {
	if(mode == INPUT || mode == INPUT_PULLUP)
		_SFR_IO8((pin >> 4) + 1) &=  ~_BV(pin & 0xF); // IN
	else if(mode == OUTPUT)
		_SFR_IO8((pin >> 4) + 1) |=  _BV(pin & 0xF); // OUT

	if(mode == INPUT_PULLUP)
		digitalWrite(pin, true);
}
