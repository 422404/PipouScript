/**
 * @file ast.c
 * Abstract Syntax Tree implementation
 * @todo tests
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ast.h"
#include "vector.h"

/**
 * Initialize a node by its type
 * !Only allocates vectors!
 * @param[in] node A pointer to the node to initialize
 */
static void ASTNode_Init(ast_node_t * node) {
    switch (node->type) {
        case NODE__ROOT_:
            node->as_root.statements = Vec_New();
            break;
        case NODE_OBJ_FIELD_NAME:
            node->as_obj_field_name.components = Vec_NewWithIncrementLength(1);
            break;
        case NODE_MSG_SEL:
            node->as_msg_sel.ident_list = Vec_NewWithIncrementLength(4);
            break;
        case NODE_OBJ_MSG_DEF:
            node->as_obj_msg_def.statements = Vec_NewWithIncrementLength(20);
            break;
        case NODE_OBJ_LITTERAL:
            node->as_obj_litteral.obj_fields = Vec_NewWithIncrementLength(15);
            break;
        case NODE_ARRAY_LITTERAL:
            node->as_array_litteral.items = Vec_NewWithIncrementLength(10);
            break;
        case NODE_BLOCK:
            node->as_block.params = Vec_NewWithIncrementLength(2);
            node->as_block.statements = Vec_NewWithIncrementLength(10);
            break;
        case NODE_DOTTED_EXPR:
            node->as_dotted_expr.components = Vec_NewWithIncrementLength(2);
            break;
        case NODE_MSG_PASS_EXPR:
            node->as_msg_pass_expr.components = Vec_NewWithIncrementLength(4);
            break;
        case NODE_OR_EXPR:
        case NODE_AND_EXPR:
        case NODE_COMP_EXPR:
        case NODE_ARITH_EXPR:
        case NODE_TERM_EXPR:
        case NODE_FACTOR_EXPR:
        case NODE_UNARY_EXPR:
        case NODE_ATOM_EXPR:
            node->as_expr.values = Vec_NewWithIncrementLength(1);
            break;
        default:
            break;
    }
}

/**
 * Allocates a new AST node
 * @param type AST node type
 * @returns    A pointer to the newly allocated node
 */
ast_node_t * ASTNode_New(ast_node_type_t type) {
    ast_node_t * node = (ast_node_t *)malloc(sizeof(ast_node_t));
    
    if (node) {
        memset(node, 0, sizeof(ast_node_t));
        node->type = type;
        ASTNode_Init(node);
    } else {
        Err_Throw(Err_New("Cannot allocate AST node"));
    }

    return node;
}

/**
 * Frees a node and its children if any
 * @param[int] node The node to free
 */
void ASTNode_Free(ast_node_t * node) {
    if (!node) Err_Throw(Err_New("NULL pointer to AST node"));
    switch (node->type) {
        case NODE__ROOT_:
            Vec_ForEach(node->as_root.statements, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_root.statements);
            break;
        case NODE_IDENTIFIER:
            free(node->as_ident.value);
            break;
        case NODE_STRING:
            free(node->as_string.value);
            break;
        case NODE_DECL:
            ASTNode_Free(node->as_decl.lval);
            ASTNode_Free(node->as_decl.rval);
            break;
        case NODE_AFFECT:
            ASTNode_Free(node->as_affect.lval);
            ASTNode_Free(node->as_affect.rval);
            break;
        case NODE_OBJ_FIELD_NAME:
            Vec_ForEach(node->as_obj_field_name.components, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_obj_field_name.components);
            break;
        case NODE_OBJ_FIELD_INIT:
            ASTNode_Free(node->as_obj_field_init.ident);
            ASTNode_Free(node->as_obj_field_init.value);
            break;
        case NODE_MSG_SEL:
            Vec_ForEach(node->as_msg_sel.ident_list, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_msg_sel.ident_list);
            break;
        case NODE_OBJ_MSG_DEF:
            Vec_ForEach(node->as_obj_msg_def.statements, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_obj_msg_def.statements);
            ASTNode_Free(node->as_obj_msg_def.selector);
            break;
        case NODE_OBJ_LITTERAL:
            Vec_ForEach(node->as_obj_litteral.obj_fields, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_obj_litteral.obj_fields);
            break;
        case NODE_ARRAY_LITTERAL:
            Vec_ForEach(node->as_array_litteral.items, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_array_litteral.items);
            break;
        case NODE_BLOCK:
            Vec_ForEach(node->as_block.params, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_block.params);
            Vec_ForEach(node->as_block.statements, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_block.statements);
            break;
        case NODE_ARRAY_ACCESS:
            ASTNode_Free(node->as_array_access.index_expr);
            break;
        case NODE_DOTTED_EXPR:
            Vec_ForEach(node->as_dotted_expr.components, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_dotted_expr.components);
            break;
        case NODE_MSG_PASS_EXPR:
            Vec_ForEach(node->as_msg_pass_expr.components, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_msg_pass_expr.components);
            break;
        case NODE_OR_EXPR:
        case NODE_AND_EXPR:
        case NODE_COMP_EXPR:
        case NODE_ARITH_EXPR:
        case NODE_TERM_EXPR:
        case NODE_FACTOR_EXPR:
        case NODE_UNARY_EXPR:
        case NODE_ATOM_EXPR:
            Vec_ForEach(node->as_expr.values, (void (*)(void *))ASTNode_Free);
            Vec_Free(node->as_expr.values);
            break;
        case NODE_STATEMENT:
            ASTNode_Free(node->as_statement.value);
            break;
        default:
            break;
    }
    free(node);
}

/**
 * Builds the string representation of an AST node
 * @param[in] node The node to build the string from
 * @returns        The string representation of the node
 */
char * ASTNode_ToString(ast_node_t * node) {
    char * string = NULL;

    if (!node) Err_Throw(Err_New("NULL pointer to AST node"));
    switch (node->type) {
        case NODE__ROOT_:
            string = strdup("ASTRootNode { }");
            break;
        case NODE_IDENTIFIER:
            string =  strdup("ASTIdentifierNode { }");
            break;
        case NODE_STRING:
            string =  strdup("ASTStringNode { }");
            break;
        case NODE_INT:
            string =  strdup("ASTIntNode { }");
            break;
        case NODE_DOUBLE:
            string =  strdup("ASTDoubleNode { }");
            break;
        case NODE_DECL:
            string =  strdup("ASTDeclNode { }");
            break;
        case NODE_AFFECT:
            string =  strdup("ASTAffectNode { }");
            break;
        case NODE_OBJ_FIELD_NAME:
            string =  strdup("ASTObjFieldNameNode { }");
            break;
        case NODE_OBJ_FIELD_INIT:
            string =  strdup("ASTObjFieldInitNode { }");
            break;
        case NODE_MSG_SEL:
            string =  strdup("ASTMsgSelNode { }");
            break;
        case NODE_OBJ_MSG_DEF:
            string =  strdup("ASTObjMsgDefNode { }");
            break;
        case NODE_OBJ_LITTERAL:
            string =  strdup("ASTObjLitteralNode { }");
            break;
        case NODE_ARRAY_LITTERAL:
            string =  strdup("ASTArrayLitteralNode { }");
            break;
        case NODE_BLOCK:
            string =  strdup("ASTBlockNode { }");
            break;
        case NODE_ARRAY_ACCESS:
            string =  strdup("ASTArrayAccessNode { }");
            break;
        case NODE_DOTTED_EXPR:
            string =  strdup("ASTDottedExprNode { }");
            break;
        case NODE_MSG_PASS_EXPR:
            string =  strdup("ASTMsgPassExprNode { }");
            break;
        case NODE_OR_EXPR:
            string =  strdup("ASTOrExprNode { }");
            break;
        case NODE_AND_EXPR:
            string =  strdup("ASTAndExprNode { }");
            break;
        case NODE_COMP_EXPR:
            string =  strdup("ASTCompExprNode { }");
            break;
        case NODE_ARITH_EXPR:
            string =  strdup("ASTArithExprNode { }");
            break;
        case NODE_TERM_EXPR:
            string =  strdup("ASTTermExprNode { }");
            break;
        case NODE_FACTOR_EXPR:
            string =  strdup("ASTFactorExprNode { }");
            break;
        case NODE_UNARY_EXPR:
            string =  strdup("ASTUnaryExprNode { }");
            break;
        case NODE_ATOM_EXPR:
            string =  strdup("ASTAtomExprNode { }");
            break;
        case NODE_STATEMENT:
            string =  strdup("ASTStatementNode { }");
            break;
    }

    return string;
}
