# mgclisp
An S-expression interpreter

Expreimenting with implementing token stream via iterator pattern, so far seems nice.
Right now functionality is limited to evaluating algebraic expressions, and can utilize
user defined variables.

     max@MaxGorenLaptop:~/mgclisp$ g++ mgclisp.cpp
     max@MaxGorenLaptop:~/mgclisp$ ./a.out
     mgclisp> (+ (/ 6 28) 271)
        275    
     mgclisp> exit
     max@MaxGorenLaptop:~/mgclisp$
     
Summing up the LOC that make up mgclisp:

     max@MaxGorenLaptop:~/mgclisp$
     mgclisp> ( + ( + 63 55) (+ (+ 78 51) 59))
        306
     mgclisp> (+ 306 22)
        328
     max@MaxGorenLaptop:~/mgclisp$

If you pass verbosity = true to the interpreter, it will also display the results of parsing the expression
as well as the evaluation steps:

     max@MaxGorenLaptop:~/mgclisp$ ./mgclisp -v
     mgclisp> (let (x 2))
     <[LPAREN, (]>
     <[LET, let]>
     <[LPAREN, (]>
     <[IDSYM, x]>
     <[NUM, 2]>
     <[RPAREN, )]>
     <[RPAREN, )]>
     x: 2
      --> 2
     mgclisp> (+ x 8)
     <[LPAREN, (]>
     <[ADD, +]>
     <[IDSYM, x]>
     <[NUM, 8]>
     <[RPAREN, )]>
     Push: ADD
     Push: 2
     Push: 8
     Pop: ADD  
     Pop: 8
     Pop: 2
     8 ADD 2
    Result: 10
      --> 10
    mgclisp> exit
    max@MaxGorenLaptop:~/mgclisp$
