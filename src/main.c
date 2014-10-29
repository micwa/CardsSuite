/**
 * Copyright (c) 2014 by Michael Wang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "wargame.h"
#include "solitgame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#define NGAMES 2

static void str_tolower(char *s)
{
    while (*s != '\0') {
        *s = tolower(*s);
        s++;
    }
}

static void game_select(char *name)
{
    printf("\n");
    str_tolower(name);                      /* Ignore case when launching game */
    
    if (strcmp(name, "war") == 0)
        war();
    else if (strcmp(name, "solitaire") == 0)
        solitaire();
    else
        printf("Not a valid game.\n");
}

int main(int argc, char *argv[])
{
    char *old_lcl = setlocale(LC_CTYPE, "en_US.utf8");
    char *games[NGAMES] = { "war", "solitaire" };
    int option = 0;

    if (argc == 1) {                        /* Prompt for game if no game specified */
        printf("Choose a game:\n\n");
        printf("    1: War\n");
        printf("    2: Solitaire\n");
        printf("    3: [Quit]\n\n");
        printf("Option: ");

        do {
            scanf("%1d", &option);
            while (getchar() != '\n')
                ;
        } while (option < 1 || option > NGAMES + 1);
        
        if (option == NGAMES + 1)
            return 0;
        game_select(games[option - 1]);
    } else if (argc == 2)
        game_select(argv[1]);
    else
        printf("Too many arguments!");

    setlocale(LC_CTYPE, old_lcl);
    return 0; 
}
