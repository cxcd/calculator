#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "calc.h"

#define PROGRAM_NAME "calc"

typedef struct parse_params {
	char curr_char;
	char *m_input;
	int curr_index;
	int expect_parens;
} parse_params;

/* Hold list of terminal symbols as type Symbol*/
enum Symbol {
	NUMBER,
	POINT,
	L_PARENS,
	R_PARENS,
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	POW,
	INVALID
};

/* Convert char to Symbol */
enum Symbol char_to_symbol(char c) {
	switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':	
		case '7':
		case '8':
		case '9':	return NUMBER;
		case '.':	return POINT;
		case '(': 	return L_PARENS;
		case ')': 	return R_PARENS;
		case '+': 	return PLUS;
		case '-': 	return MINUS;
		case '*': 	return MULTIPLY;
		case '/':	return DIVIDE;
		case '^':	return POW;
		default: 	return INVALID;
	}
}

/* Print error message and exit */
void err(const char *msg) {
	fprintf(stderr, "Error: %s.\n", msg);
	exit(1);
}
/* Print usage message and exit */
void usage() {
	printf("Usage: %s expression\n", PROGRAM_NAME);
	exit(0);
}

/* Increment the input and read the current symbol*/
void consume(parse_params *params_ptr) {
	params_ptr->curr_index = params_ptr->curr_index + 1;
	params_ptr->curr_char = params_ptr->m_input[params_ptr->curr_index];
}

/* Return the current symbol in the input without incrementing */
enum Symbol peek(parse_params *params_ptr) {
	return char_to_symbol(params_ptr->m_input[params_ptr->curr_index]);
}

/* Recursive Descent Parser */
double parse_expression(parse_params *params_ptr);

double parse_number(parse_params *params_ptr) {
	double result = 0, fractional = 0;
	int length = 0;
	while(peek(params_ptr) == NUMBER) {
		result = result * 10 + (params_ptr->curr_char - '0');
		consume(params_ptr);
	}
	if (peek(params_ptr) == POINT) {
		consume(params_ptr);
		if (peek(params_ptr) != NUMBER) {
			err("Syntax: Unexpected decimal separator");
		}
		while(peek(params_ptr) == NUMBER) {
			fractional = fractional * 10 + (params_ptr->curr_char - '0');
			length++;
			consume(params_ptr);
		}
	}
	if (fractional > 0) {
		result = result + (fractional/pow(10, length));
	}
	if (peek(params_ptr) == L_PARENS) {
		err("Syntax: Unexpected open parenthesis");
	} else if (peek(params_ptr) == R_PARENS && !params_ptr->expect_parens) {
		err("Syntax: Unexpected closed parenthesis");
	}
	return result;
}

double parse_factor(parse_params *params_ptr) {
	int sign = 1;
	double expression = 0;
	if (peek(params_ptr) == MINUS) {
		consume(params_ptr);
		sign = -1;
	} else {
		sign = 1;
	}	
	if (peek(params_ptr) == NUMBER) {
		return sign * parse_number(params_ptr);
	} else if (peek(params_ptr) == L_PARENS) {
		params_ptr->expect_parens = 1;
		consume(params_ptr);
		expression = parse_expression(params_ptr);
		if (peek(params_ptr) != R_PARENS) {
			err("Syntax: Expected closed parenthesis");
		}
		consume(params_ptr);
		params_ptr->expect_parens = 0;
		return sign * expression;
	} else if (peek(params_ptr) == POINT) {
		err("Syntax: Unexpected decimal separator");
	} else {
		err("Syntax");
	}
}

double parse_exponent(parse_params *params_ptr) {
	double factor = parse_factor(params_ptr);
	while (1) {
		if (peek(params_ptr) == POW) {
			consume(params_ptr);
			factor = pow(factor, parse_factor(params_ptr));
		} else {
			return factor;
		}
	}
}

double parse_term(parse_params *params_ptr) {
	double exponent = parse_exponent(params_ptr);
	while (1) {
		if (peek(params_ptr) == MULTIPLY) {
			consume(params_ptr);
			exponent = exponent * parse_exponent(params_ptr);
		} else if (peek(params_ptr) == DIVIDE) {
			consume(params_ptr);
			exponent = exponent / parse_exponent(params_ptr);
		} else {
			return exponent;
		}
	}
}

double parse_expression(parse_params *params_ptr) {
	double term = parse_term(params_ptr);
	while(1) {
		if (peek(params_ptr) == PLUS) {
			consume(params_ptr);
			term = term + parse_term(params_ptr);
		} else if (peek(params_ptr) == MINUS) {
			consume(params_ptr);
			term = term - parse_term(params_ptr);
		} else {
			return term;
		}
	}
}

/* Remove white spaces from input */
char *remove_whitespace(char *input) {
    int i = 0, j = 0;
    char *output = input;
    for (i = 0, j = 0; i < strlen(input); i++, j++) {
        if (input[i]!=' ') {
            output[j]=input[i];
		} else {
            j--;
		}			
    }
    output[j]=0;
    return output;
}
/* Initialize the struct and wrap the call */
double calc_parse(char *arg) {
	char *init_input = remove_whitespace(arg);
	parse_params params = {init_input[0], init_input, 0, 0};
	return parse_expression(&params);
}