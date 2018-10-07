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
void consume(parse_params *paramsPtr) {
	paramsPtr->curr_index = paramsPtr->curr_index + 1;
	paramsPtr->curr_char = paramsPtr->m_input[paramsPtr->curr_index];
}

/* Return the current symbol in the input without incrementing */
enum Symbol peek(parse_params *paramsPtr) {
	return char_to_symbol(paramsPtr->m_input[paramsPtr->curr_index]);
}

/* Recursive Descent Parser */
double parse_expression(parse_params *paramsPtr);

double parse_number(parse_params *paramsPtr) {
	double num = 0, decimal = 0;
	int length = 0;
	while(peek(paramsPtr) == NUMBER) {
		num = num * 10 + (paramsPtr->curr_char - '0');
		consume(paramsPtr);
	}
	if (peek(paramsPtr) == POINT) {
		consume(paramsPtr);
		if (peek(paramsPtr) != NUMBER) {
			err("Syntax: Unexpected decimal point");
		}
		while(peek(paramsPtr) == NUMBER) {
			decimal = decimal * 10 + (paramsPtr->curr_char - '0');
			length++;
			consume(paramsPtr);
		}
	}
	if (decimal > 0) {
		num = num + (decimal/pow(10, length));
	}
	if (peek(paramsPtr) == L_PARENS) {
		err("Syntax: Unexpected open parenthesis");
	} else if (peek(paramsPtr) == R_PARENS && !paramsPtr->expect_parens) {
		err("Syntax: Unexpected closed parenthesis");
	}
	return num;
}

double parse_factor(parse_params *paramsPtr) {
	int sign = 1;
	double expression = 0;
	if (peek(paramsPtr) == MINUS) {
		consume(paramsPtr);
		sign = -1;
	} else {
		sign = 1;
	}	
	if (peek(paramsPtr) == NUMBER) {
		return sign * parse_number(paramsPtr);
	} else if (peek(paramsPtr) == L_PARENS) {
		paramsPtr->expect_parens = 1;
		consume(paramsPtr);
		expression = parse_expression(paramsPtr);
		if (peek(paramsPtr) != R_PARENS) {
			err("Syntax: Expected closed parenthesis");
		}
		consume(paramsPtr);
		paramsPtr->expect_parens = 0;
		return sign * expression;
	} else if (peek(paramsPtr) == POINT) {
		err("Syntax: Unexpected decimal point");
	} else {
		err("Syntax");
	}
}

double parse_exponent(parse_params *paramsPtr) {
	double factor = parse_factor(paramsPtr);
	while (1) {
		if (peek(paramsPtr) == POW) {
			consume(paramsPtr);
			factor = pow(factor, parse_factor(paramsPtr));
		} else {
			return factor;
		}
	}
}

double parse_term(parse_params *paramsPtr) {
	double exponent = parse_exponent(paramsPtr);
	while (1) {
		if (peek(paramsPtr) == MULTIPLY) {
			consume(paramsPtr);
			exponent = exponent * parse_exponent(paramsPtr);
		} else if (peek(paramsPtr) == DIVIDE) {
			consume(paramsPtr);
			exponent = exponent / parse_exponent(paramsPtr);
		} else {
			return exponent;
		}
	}
}

double parse_expression(parse_params *paramsPtr) {
	double term = parse_term(paramsPtr);
	while(1) {
		if (peek(paramsPtr) == PLUS) {
			consume(paramsPtr);
			term = term + parse_term(paramsPtr);
		} else if (peek(paramsPtr) == MINUS) {
			consume(paramsPtr);
			term = term - parse_term(paramsPtr);
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
double parse(char *arg) {
	char *init_input = remove_whitespace(arg);
	parse_params params = {init_input[0], init_input, 0, 0};
	return parse_expression(&params);
}