#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* Each of our text files contains 1000 words. */
#define LIST_SIZE       1000
#define FNAME_SIZE      6
#define PROMPT_SIZE     128
#define MAX_WORD_LENGTH 8
#define MIN_WORD_LENGTH 5

#define EXACT   2               /* Right letter; right place. */
#define CLOSE   1               /* Right letter; wrong place. */
#define WRONG   0               /* Wrong letter. */

/* ANSI color codes for boxed in letters.*/
#define GREEN   "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
#define YELLOW  "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
#define RED     "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
#define RESET   "\e[0;39m"

static char *get_guess(size_t wlen)
{
    char *guess = NULL;
    size_t glen = 0;
    ssize_t rc = 0;

    char prompt[PROMPT_SIZE];

    snprintf(prompt, PROMPT_SIZE, "Input a %zu-letter word: ", wlen);

    do {
        fputs(prompt, stdout);
        rc = getline(&guess, &glen, stdin);

        if (rc == -1) {
            break;
        }
        guess[strcspn(guess, "\n")] = '\0';
    } while (strlen(guess) != wlen);

    return guess;
}

static bool guess_exists(const char *restrict guess, size_t lsize, size_t wlen, const char list[lsize][wlen]) 
{
    for (size_t i = 0; i < lsize ; ++i) {
        if (!strcasecmp(guess, list[i])) {
            return true;
        }
    }

    return false;
}

static size_t sum_array(size_t size, const size_t arr[size])
{
    size_t sum = 0;

    for (size_t i = 0; i < size; ++i) {
        sum += arr[i];
    }

    return sum;
}

static size_t calculate_word_score(const char *restrict guess, size_t *restrict status,
                                   const char *restrict choice)
{
    size_t i;

    for (i = 0; guess[i]; ++i) {
        for (size_t j = 0; choice[j]; ++j) {
            if (choice[j] == guess[i]) {
                if (i == j) {
                    status[i] = EXACT;
                } else if (status[i] != EXACT) {
                    status[i] = CLOSE;
                }
            }
        }
    }

    return sum_array(i, status);
}

static void print_word(const char *restrict guess, size_t wlen,
                       const size_t *restrict status)
{
    for (size_t i = 0; i < wlen; ++i) {
        switch (status[i]) {
            case EXACT:
                printf(GREEN "%c" RESET, guess[i]);
                break;

            case CLOSE:
                printf(YELLOW "%c" RESET, guess[i]);
                break;

            default:
                printf(RED "%c" RESET, guess[i]);
                break;
        }
    }
    putchar('\n');
}

static inline bool validate_wlen(size_t wlen)
{
    return wlen >= MIN_WORD_LENGTH && wlen <= MAX_WORD_LENGTH;
}

static bool run_game(size_t lsize, size_t wlen, const char options[lsize][wlen + 1], const char *restrict choice)
{
    /* Allow one more guess than the length of the word. */
    const size_t guesses = wlen + 1;
    bool won = false;

    printf(GREEN "This is WORDLE50" RESET "\n"
           "You have %zu tries to guess the %zu-letter word I'm thinking of\n",
           guesses, wlen);

    for (size_t i = 0; i < guesses; ++i) {
        char *guess = get_guess(wlen);

        /* Check if guess is in the list. */
        while (!guess_exists(guess, LIST_SIZE, wlen + 1, options)) {
            puts("The word is not in list.");
            free(guess);
            guess = get_guess(wlen);
        }
        
        /* Array to hold guess status. */
        size_t status[wlen];

        /* Set all elements of status array initially to 0, aka WRONG. */
        for (size_t i = 0; i < wlen; ++i) {
            status[i] = 0;
        }

        size_t score = calculate_word_score(guess, status, choice);
        printf("Guess %zu: ", i + 1);
        print_word(guess, wlen, status);
        free(guess);
        
        /* If they guessed it exactly right, set terminate loop. */
        if (score == EXACT * wlen) {
            won = true;
            break;
        }
    }

    return won;
}

int main(int argc, char **argv)
{
    if (argc != 2 || !validate_wlen(strtol(argv[1], NULL, 10))) {
        fprintf(stderr, "Usage: %s <k> (5-8)\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open correct file, each file has exactly LIST_SIZE words. */
    const size_t wlen = (size_t) strtol(argv[1], NULL, 10);
    char wl_filename[FNAME_SIZE];

    snprintf(wl_filename, FNAME_SIZE, "%zu.txt", wlen);

    FILE *const wlist = fopen(wl_filename, "r");

    if (!wlist) {
        perror("fopen()");
        return EXIT_FAILURE;
    }

    /* Load word file into an array of size LIST_SIZE. */
    char options[LIST_SIZE][wlen + 1];

    for (size_t i = 0; i < LIST_SIZE; ++i) {
        fscanf(wlist, "%s", options[i]);
    }
    
    /* Pseudorandomly select a word for this game. */
    srand(time(NULL));
    const char *const choice = options[rand() % LIST_SIZE];

    run_game(LIST_SIZE, wlen, options, choice) ? puts("You won!") : printf("The target word was: %s\n", choice);
    fclose(wlist);
    return EXIT_SUCCESS;
}
