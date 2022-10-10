#include <stdio.h>
#include "mpc.h"

/* This follows the tutorial at https://buildyourownlisp.com/ */

/* This is a polish notation arithmetic, where the operator is at the start, before the numbers */

/* Interactive prompt that repeatedly writes a message and then waits for input */


/* Declare a buffer for the user input */
static char input[2048];

/* Declare New lval Struct this is LISP Value */
/* It's type is an integer, which we will polulate using enum */
/* It can be a number, or it can be an error */
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Create enumeration of the possible lval types, lval number and lval error */
/* Now LVAL_NUM = 0 and LVAL_ERR = 1, but we can use their names instead of 0 and 1, to be more readable */
enum { LVAL_NUM, LVAL_ERR };
/* Create Enumeration for all the error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* Print an "lval" depending on what it is */
void lval_print(lval v) {
  switch (v.type) {
    /* In the case the type is a number print it */
    /* Then 'break' out of the switch. */
    case LVAL_NUM: printf("%li", v.num); break;

    /* In the case the type is an error */
    case LVAL_ERR:
      /* Check what type of error it is and print it */
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero!");
      }
      if (v.err == LERR_BAD_OP)   {
        printf("Error: Invalid Operator!");
      }
      if (v.err == LERR_BAD_NUM)  {
        printf("Error: Invalid Number!");
      }
    break;
  }
}
/* Print an "lval" followed by a newline */
void lval_println(lval v) { lval_print(v); putchar('\n'); }

/* Create a new number type lval */
lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

/* Create a new error type lval */
lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}


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
	lval eval_op(lval x, char* op, lval y) {
		/* If we have an error, return it */
		if (x.type == LVAL_ERR) { return x; }
		if (y.type == LVAL_ERR) { return y; }
		
		/* Otherwise do maths on the numbers */
		if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
		if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
		if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
		if (strcmp(op, "/") == 0) {
			/* This is just an return IF y.num, THEN (?) lval_err(LERR_DIV_ZERO), ELSE (:) lval_num(x.num / y_num) */
			return y.num == 0
				? lval_err(LERR_DIV_ZERO)	
				: lval_num(x.num / y.num);
			}
			
		/* If we can't identify an operator, it must have been a bad one */
		return lval_err(LERR_BAD_OP);
	}	
	
	/* atoi converts char* (a string) to an int */
	/* strcmp takes two char* and if they are equal it returns 0 */
	/* strstr takes an input two char* and returns a pointer to the location of the second in the first
	or 0 if the second is not a sub-string of the  first */
	lval eval(mpc_ast_t* t) {

		/* If tagged as number return it directly. */
		/* This is the base case in recursion, the final recursive step */
		if (strstr(t->tag, "number")) {
			/* Check if there is some error in conversion */
			errno = 0;
			/* We check if we find t between NULL and 10) */
			long x = strtol(t->contents, NULL, 10);
			return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
		}

	
	  /* The operator is always second child. */
	  char* op = t->children[1]->contents;

	  /* We store the third child in `x` */
	  lval x = eval(t->children[2]);

	  /* Iterate the remaining children and combining. */
	  int i = 3;
	  /* We iterate through every child, aslong as it is an expression */
	  while (strstr(t->children[i]->tag, "expr")) {
		/* We then evaluate x, the second child, with the operation specified */
		/* with whatever we get child eval(child[i]) */
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
			/* mpc_ast_print(r.output); */
			/* Load AST from output */
			/* mpc_ast_t* a = r.output; */
#
			/* Get the eval of parser output, then print using our special lval print function */
			lval result = eval(r.output);
			lval_println(result);

			/* Perform this cleanup at the end */
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






