/**
 * @file main.c
 * Interpreter entrypoint
 */
#include "misc.h"
#include "eval.h"

/**
 * Interpreter entrypoint
 */
int main(int argc, char ** argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    return Eval_REPL();
}
