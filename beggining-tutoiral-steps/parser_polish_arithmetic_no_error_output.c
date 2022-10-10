#include <stdio.h>
#include "mpc.h"

/* This is a polish notation arithmetic, where the operator is at the start, before the numbers */

/* Interactive prompt that repeatedly writes a message and then waits for input */


/* Declare a buffer for the user input */
static char input[2048];

int main(int argc, char** argv) {
	
	
	
	/* Create Some Parsers */
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr     = mpc_new("expr");
	mpc_parser_t* Lispy    = mpc_new("lispy");

	/* Regular expressions are initiated with / and ended with / */
	/*  a?   - the charecter a is optional */
	/*  a*   - zero or more of charecter a required */
	/*  a+   - one or more of charecter a are required */
	/*  ^    - the start of input is required */
	/*  $    - the end of input it required */
	/*  [a-f] - any charecter in the range is required */

	/* Define them with the following Language */
	mpca_lang(MPCA_LANG_DEFAULT,
	  "                                                     \
		number   : /-?[0-9]+/ ;                             \
		operator : '+' | '-' | '*' | '/' ;                  \
		expr     : <number> | '(' <operator> <expr>+ ')' ;  \
		lispy    : /^/ <operator> <expr>+ /$/ ;             \
	  ",
	  Number, Operator, Expr, Lispy);

	/* So we can see that number has an optional - sign, then it must have at least 1 digit between 0 and 9 */
	/* The operators can only be 4 exact things */
	/* An expression can be a number, or it can be a '(' then an operator then one or more expressions, then a '); */
	/* Then lispy is an operaotr and one or more expressions */





	/* Here we evaluate the code */
	
	/* Use operator string to see which operation to perform */
	long eval_op(long x, char* op, long y) {
		if (strcmp(op, "+") == 0) { return x + y; }
		if (strcmp(op, "-") == 0) { return x - y; }
		if (strcmp(op, "*") == 0) { return x * y; }
		if (strcmp(op, "/") == 0) { return x / y; }
		return 0;
	}	
	
	/* atoi converts char* (a string) to an int */
	/* strcmp takes two char* and if they are equal it returns 0 */
	/* strstr takes an input two char* and returns a pointer to the location of the second in the first
	or 0 if the second is not a sub-string of the  first */
	long eval(mpc_ast_t* t) {

		/* If tagged as number return it directly. */
		/* This is the base case in recursion, the final recursive step */
		if (strstr(t->tag, "number")) {
			return atoi(t->contents);
		}

	
	  /* The operator is always second child. */
	  char* op = t->children[1]->contents;

	  /* We store the third child in `x` */
	  long x = eval(t->children[2]);

	  /* Iterate the remaining children and combining. */
	  int i = 3;
	  /* We iterate through every child, aslong as it is an expression */
	  while (strstr(t->children[i]->tag, "expr")) {
		/* We then evaluate x, the second child, with the operation specified
		with whatever we get child eval(child[i]) */
		/* So if the child is a number then we just eval two numbers */
		/* But if the child is another expression we eval it first */
		/* This clearly defines the order of arithmetic operations rightmost first */
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	  }

	  return x;
	}

	/* Here we do our version and exit stuff */
	puts("CHISP Version 0.0.0.0.1");
	puts("Press Crtl+c to Exit\n");
	
	/* Never ending loop forms out interactive prompt */
	while (1) {
		
		/*	Output our prompt */
		fputs("chisp> ", stdout);
		
		/* Read a line of input (of max size 2048) */
		fgets(input, 2048, stdin);
		
		/* Echo input to user */
		/* printf("You are a %s", input);*/
		
		/* Parse the user input using the polish arithmetic parser */
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			/* On success print the AST (We can now comment this code out) */
			mpc_ast_print(r.output);
			/* Load AST from output */
			mpc_ast_t* a = r.output;
#
			/* Get the eval of parser output, then print using %li specifier for long print */
			long result = eval(a);
			printf("%li\n", result);
			/*
			This is justto explore what the parser output looks like
			printf("Tag: %s\n", a->tag);
			printf("Contents: %s\n", a->contents);
			printf("Number of children: %i\n", a->children_num);
			*/
  
			mpc_ast_delete(r.output);
		} else {
			/* Otherwise print the error */
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}
		
		
		
	}
	
	/* Undefine and Delete our Parsers */
	mpc_cleanup(4, Number, Operator, Expr, Lispy);

	return 0;
}






