/**
 * @file native_invokation_context.h
 * @todo Implementation
 */
#pragma once
#include "nanbox.h"

/**
 * Instanciates a new native invokation context
 * @param scope  An array-object that contains the args of the block indexed
 *               and the outer scopes in its prototype chain 
 * @param caller The calling block
 * @return       The instanciated context
 */
nanbox_t NativeInvokationContext_New(nanbox_t scope, nanbox_t caller);

/**
 * Return a ref to an argument passed to the block
 * @param context The native invokation context of the block
 * @param index   The index of the argument of the block
 * @retval        The reference to the argument
 * @retval        nanbox_null() if an error occured
 *                (like an attempted out of bound read)
 */
nanbox_t NativeInvokationContext_GetArg(nanbox_t context, int index);

/**
 * Return a ref to a variable in an outer scope of the block
 * @param context The native invokation context of the block
 * @param name    The name of the variable
 * @retval        The reference to the variable
 * @retval        nanbox_null() if an error occured
 *                (if the variable doesn't exists)
 */
nanbox_t NativeInvokationContext_GetVar(nanbox_t context, char * name);

/**
 * @todo Stubbed, returns nanbox_null()
 */
nanbox_t NativeInvokationContext_GetCaller(nanbox_t context);

/**
 * Raise an exception in the block
 * It will propagate through the call stack
 * @param context The native invokation context of the block
 * @param error   The object to use as an exception
 */
void NativeInvokationContext_Raise(nanbox_t context, nanbox_t error);

/**
 * Returns the raised exception object
 * @param context the native invokation context of the block
 * @retval        The exception object raised
 * @retval        nanbox_null() if no exception has been raised 
 */
nanbox_t NativeInvokationContext_GetRaisedError(nanbox_t context);

/**
 * Tells whether an exception was raised in the block
 * @param context The native invokation context of the block
 * @return        Whether the block has raised
 */
bool NativeInvokationContext_HasRaised(nanbox_t context);
