/**
 * @file eval.h
 * Evaluation of user input in interactive mode
 */
#pragma once

/**
 * Puts the interpreter in REPL mode and waits for input
 * @retval 0 on termination without error
 * @retval Non-zero on failure
 */
int Eval_REPL();
