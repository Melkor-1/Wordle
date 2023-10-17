#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* Each of our text files contains 1000 words. */
#define LIST_SIZE  1000
#define FNAME_SIZE 6

#define PROMPT_SIZE 128

#define EXACT   2     /* Right letter; right place. */
#define CLOSE   1     /* Right letter; wrong place. */
#define WRONG   0     /* Wrong letter. */

/* ANSI color codes for boxed in letters.*/
#define GREEN   "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define YELLOW  "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
#define RED     "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define RESET   "\e[0;39m"

static char *get_guess (size_t wlen)
{
    char *guess = NULL;
    size_t glen = 0;
    ssize_t rc = 0;

    char prompt[PROMPT_SIZE];
    snprintf (prompt, PROMPT_SIZE, "Input a %zu-letter word: ", wlen);

    do {
        printf(prompt);
        rc = getline(&guess, &glen, stdin);

        if (rc == -1) {
            break;
        }
        guess[strcspn(guess, "\n")] = '\0';
    } while (strlen(guess) != wlen);

    return guess;
}

static size_t check_word (const char *restrict guess, size_t *restrict status, const char *restrict choice)
{
    size_t score = 0;

    for (size_t i = 0; guess[i]; ++i) {
        for (size_t j = 0; choice[j]; ++j) {
            if (choice[j] == guess[i]) {
                status[i] = (i == j) ? EXACT : CLOSE;
                score += status[i];
            } 
        }
    }

    return score;
}

static void print_word (const char *restrict guess, int wlen, const size_t *restrict status)
{
    for (size_t i = 0; i < wlen; ++i) {
        switch (status[i]) {
            case EXACT:
                printf (GREEN"%c"RESET, guess[i]);
                break;

            case CLOSE:
                printf (YELLOW"%c"RESET, guess[i]);
                break;

            default:
                printf (RED"%c"RESET, guess[i]);
                break;
        }
    }
    putchar('\n');
    fflush(stdout);
}

static inline bool validate_wlen (size_t wlen)
{
    return wlen >= 5 && wlen <= 8;
}

int main (int argc, char **argv)
{
    if (argc != 2 || !validate_wlen (strtol (argv[1], NULL, 10))) {
        fprintf (stderr, "Usage: %s <k> (5-8)\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open correct file, each file has exactly LISTSIZE words
    const size_t wlen = (size_t) atoi (argv[1]);
    char wl_filename[FNAME_SIZE];

    snprintf(wl_filename, FNAME_SIZE, "%zu.txt", wlen);

    FILE *const wlist = fopen(wl_filename, "r");
    
    if (!wlist) {
        perror ("fopen()");
        return EXIT_FAILURE;
    }

    /* Load word file into an array of size LISTSIZE. */
    char options[LIST_SIZE][wlen + 1];

    for (size_t i = 0; i < LIST_SIZE; ++i) {
        fscanf(wlist, "%s", options[i]);
    }

    /* Pseudorandomly select a word for this game. */
    srand(time(NULL));
    char *const choice = options[rand() % LIST_SIZE];

    /* Allow one more guess than the length of the word. */
    const size_t guesses = wlen + 1;
    bool won = false;

    printf (GREEN"This is WORDLE50"RESET"\n"
           "You have %zu tries to guess the %zu-letter word I'm thinking of\n",
           guesses,
           wlen);

    /* Main game loop, one iteration for each guess. */
    for (size_t i = 0; i < guesses; ++i) {
        /* Obtain user's guess. */
        char *const guess = get_guess (wlen);

        /* Array to hold guess status, initially set to zero. */
        size_t status[wlen];

        /* Set all elements of status array initially to 0, aka WRONG. */
        memset (status, WRONG, wlen);

        /* Calculate score for the guess. */
        size_t score = check_word (guess, status, choice);

        printf ("Guess %zu: ", i + 1);
        print_word(guess, wlen, status);
        fflush (stdout);
        free(guess);
        /* If they guessed it exactly right, set terminate loop. */
        if (score == EXACT * wlen) {
            won = true;
            break;
        }
    }

    won ? puts("You won!") : printf("The target word was: %s\n", choice);
    return EXIT_SUCCESS;
}

