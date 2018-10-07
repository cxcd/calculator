# Simple Recursive Descent Parser Calculator

A calculator made using recursive descent parsing. Supports positive and negative decimal numbers, and the operations addition (+), subtraction (-), division (/), multiplication (\*) and exponentiation (^). Implicit multiplication has been purposefully left out because that is the convention and many find it confusing. Takes a char* as input (eg. "12.2/32+1") and returns a double. Uses the following grammar:

E --> T + E | T - E | T  
T --> T * X | T / X | X  
X --> X ^ F | F  
F --> (E) | number

## Usage
Include the "calc.h" header file in any program and call the parser using the function "parse()", for example: 

```
int main() {
  printf ("Result: %g\n", "12.2/32+(2^4)");
  return 0;
}
```
