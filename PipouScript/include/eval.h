/**
 * @file eval.h
 * Evaluation of user input in interactive mode
 */
#pragma once

/**
 * Starts the Read-Eval-Print-Loop
 * @retval 0 if returning because the user requested it
 * @retval Non-zero if an error caused the termination of the loop 
 */
int EvalLoop();
