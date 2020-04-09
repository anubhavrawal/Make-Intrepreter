#ifndef __FAKE_H__
#define __FAKE_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct recipe
{
    int dep_count;
    int cmd_count;
    char *target;
    char **deps;
    char **commands;
   
}recipe_t;

typedef enum {RULE_START, COMMANDS} parser;

size_t parser_file(recipe_t ***, FILE *);
void cleanup(recipe_t **, size_t);

#endif