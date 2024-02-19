/*
        EXECUTING AN APPLICATION USING EXEC COMMANDS

    The execvp() family of commands can be used to execute an
    application from a process. The system call execvp()
    replaces the executing process by a new process image
    which executes the application specified as its parameter.
    Arguments can also be specified. Refer to online man pages.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>

main()
{
    char *arglist[3];

    /*  "cal" is an application which shows the calendar of the
        current year and month. "cal" with an argument specifying
        year (for example "cal 1999") shows the calendar of that
        year. Try out the "cal" command from your command prompt.

        Here we execute "cal 2012" using the execvp() system call.
        Note that we specify "cal" in the first two arguments. The
        reason for this is given in the online man pages for execvp()
    */

    /* Initialize the argument list */
    arglist[0] = (char *)malloc(4 * sizeof(char));
    strcpy(arglist[0], "cal");
    arglist[1] = (char *)malloc(5 * sizeof(char));
    strcpy(arglist[1], "2012");
    arglist[2] = NULL;

    /* Call execvp */
    execvp("cal", arglist);

    /* The execvp() system call does not return. Note that the
       following statement will not be executed.
    */

    printf("This statement is not executed if execvp succeeds.\n");
}

/*
                            2012
      January               February               March
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa
 1  2  3  4  5  6  7            1  2  3  4               1  2  3
 8  9 10 11 12 13 14   5  6  7  8  9 10 11   4  5  6  7  8  9 10
15 16 17 18 19 20 21  12 13 14 15 16 17 18  11 12 13 14 15 16 17
22 23 24 25 26 27 28  19 20 21 22 23 24 25  18 19 20 21 22 23 24
29 30 31              26 27 28 29           25 26 27 28 29 30 31


       April                  May                   June
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa
 1  2  3  4  5  6  7         1  2  3  4  5                  1  2
 8  9 10 11 12 13 14   6  7  8  9 10 11 12   3  4  5  6  7  8  9
15 16 17 18 19 20 21  13 14 15 16 17 18 19  10 11 12 13 14 15 16
22 23 24 25 26 27 28  20 21 22 23 24 25 26  17 18 19 20 21 22 23
29 30                 27 28 29 30 31        24 25 26 27 28 29 30


        July                 August              September
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa
 1  2  3  4  5  6  7            1  2  3  4                     1
 8  9 10 11 12 13 14   5  6  7  8  9 10 11   2  3  4  5  6  7  8
15 16 17 18 19 20 21  12 13 14 15 16 17 18   9 10 11 12 13 14 15
22 23 24 25 26 27 28  19 20 21 22 23 24 25  16 17 18 19 20 21 22
29 30 31              26 27 28 29 30 31     23 24 25 26 27 28 29
                                            30

      October               November              December
Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa  Su Mo Tu We Th Fr Sa
    1  2  3  4  5  6               1  2  3                     1
 7  8  9 10 11 12 13   4  5  6  7  8  9 10   2  3  4  5  6  7  8
14 15 16 17 18 19 20  11 12 13 14 15 16 17   9 10 11 12 13 14 15
21 22 23 24 25 26 27  18 19 20 21 22 23 24  16 17 18 19 20 21 22
28 29 30 31           25 26 27 28 29 30     23 24 25 26 27 28 29
                                            30 31
*/