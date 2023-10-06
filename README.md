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
