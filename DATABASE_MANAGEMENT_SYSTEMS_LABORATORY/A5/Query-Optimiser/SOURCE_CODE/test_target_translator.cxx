/**********************************************************
                    Group Project Information:  
-----------------------------------------------------------
                    Group Members:
                    
                    1. Aritra Chakraborty - 21CS10009
                    2. Bratin Mondal - 21CS10016
                    3. Sarika Bishnoi - 21CS10058
                    4. Anish Datta - 21CS30006
                    5. Somya Kumar - 21CS30050
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

#include "test_translator.h"

int main(int argc, char *argv[])
{
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}