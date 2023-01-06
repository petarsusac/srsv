#ifndef _CALL_H_
#define _CALL_H_

#include <stdio.h>
#include <stdlib.h>

/* makro koji pojednostavljuje poziv funkcija uz provjeru grešaka */
#define WARN 0
#define STOP 1

#define CALL(ACT,FUNC,...)      \
do {                            \
    if (FUNC(__VA_ARGS__)) {    \
        perror(#FUNC);          \
        if (ACT == STOP)        \
            exit(1);            \
    }                           \
} while(0)

#endif