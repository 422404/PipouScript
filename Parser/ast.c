/**
 * @file ast.c
 * Abstract Syntax Tree implementation
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"
#include "ast.h"
#include "vector.h"
#include "str.h"

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
        case NODE_EQ_EXPR:
        case NODE_COMP_EXPR:
        case NODE_ARITH_EXPR:
        case NODE_TERM_EXPR:
        case NODE_FACTOR_EXPR:
        case NODE_UNARY_EXPR:
            node->as_expr.values = Vec_NewWithIncrementLength(1);
            break;
        case NODE_IDENTIFIER:
        case NODE_STRING:
        case NODE_INT:
        case NODE_DOUBLE:
        case NODE_DECL:
        case NODE_AFFECT:
        case NODE_OBJ_FIELD_INIT:
        case NODE_ARRAY_ACCESS:
        case NODE_STATEMENT:
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
    if (node) {
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
            case NODE_EQ_EXPR:
            case NODE_COMP_EXPR:
            case NODE_ARITH_EXPR:
            case NODE_TERM_EXPR:
            case NODE_FACTOR_EXPR:
            case NODE_UNARY_EXPR:
                Vec_ForEach(node->as_expr.values, (void (*)(void *))ASTNode_Free);
                Vec_Free(node->as_expr.values);
                break;
            case NODE_STATEMENT:
                ASTNode_Free(node->as_statement.value);
                break;
            case NODE_INT:
            case NODE_DOUBLE:
                break;
        }
        free(node);
    }
}

static string * ASTNode_Spaces(size_t indent) {
    string * str = Str_New(""), * str2;

    for (size_t i = 0; i < indent; i++) {
        str2 = Str_New(" ");
        Str_Append(str, str2);
        Str_Free(str2);
    }
    return str;
}

static string * ASTNode_ToStringIndent(ast_node_t * node, size_t indent) {
    #define APPEND_FREE(str1, str2) Str_Append(str1, str2);Str_Free(str2)
    string * str, * str2 = NULL;
    char buf[256];

    if (!node) Err_Throw(Err_New("NULL pointer to AST node"));
    switch (node->type) {
        case NODE__ROOT_:
            str = Str_New("ASTRootNode {\n");

            for (size_t i = 0; i < Vec_GetLength(node->as_root.statements); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_root.statements, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_root.statements) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_IDENTIFIER:
            snprintf(buf, 256, "ASTIdentifierNode { %s }", node->as_ident.value);
            str = Str_New(buf);
            break;
        case NODE_STRING:
            snprintf(buf, 256, "ASTStringNode { \"%s\" }", node->as_string.value);
            str = Str_New(buf);
            break;
        case NODE_INT:
            snprintf(buf, 256, "ASTIntNode { %d }", node->as_int.value);
            str = Str_New(buf);
            break;
        case NODE_DOUBLE:
            snprintf(buf, 256, "ASTDoubleNode { %lf }", node->as_double.value);
            str = Str_New(buf);
            break;
        case NODE_DECL:
            str = Str_New("ASTDeclNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("lval=");
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_decl.lval, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("rval=");
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_decl.rval, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_AFFECT:
            str = Str_New("ASTAffectNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("lval=");
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_affect.lval, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("rval=");
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_affect.rval, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_OBJ_FIELD_NAME:
            str = Str_New("ASTObjFieldNameNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("is_msg_name=");
            APPEND_FREE(str, str2);
            str2 = Str_New(node->as_obj_field_name.is_msg_name ? "true" : "false");
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_obj_field_name.components); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_obj_field_name.components, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_obj_field_name.components) -1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_OBJ_FIELD_INIT:
            str = Str_New("ASTObjFieldInitNode { }"); /// @todo finish
            break;
        case NODE_MSG_SEL:
            str = Str_New("ASTMsgSelNode { }"); /// @todo finish
            break;
        case NODE_OBJ_MSG_DEF:
            str = Str_New("ASTObjMsgDefNode { }"); /// @todo finish
            break;
        case NODE_OBJ_LITTERAL:
            str = Str_New("ASTObjLitteralNode { }"); /// @todo finish
            break;
        case NODE_ARRAY_LITTERAL:
            str = Str_New("ASTArrayLitteralNode {\n");

            for (size_t i = 0; i < Vec_GetLength(node->as_array_litteral.items); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_array_litteral.items, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_array_litteral.items) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_BLOCK:
            str = Str_New("ASTBlockNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("params={\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_block.params); i++) {
                str2 = ASTNode_Spaces(indent + 8);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_block.params, i), indent + 8);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_block.params) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("},\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_block.statements); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_block.statements, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_block.statements) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_ARRAY_ACCESS:
            str = Str_New("ASTArrayAccessNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_array_access.index_expr, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_DOTTED_EXPR:
            str = Str_New("ASTDottedExprNode {\n");

            for (size_t i = 0; i < Vec_GetLength(node->as_dotted_expr.components); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_dotted_expr.components, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_MSG_PASS_EXPR:
            str = Str_New("ASTMsgPassExprNode {\n");

            for (size_t i = 0; i < Vec_GetLength(node->as_msg_pass_expr.components); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                if (i == 0) {
                    str2 = Str_New("receiver=");
                    APPEND_FREE(str, str2);
                }
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_msg_pass_expr.components, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_OR_EXPR:
            str = Str_New("ASTOrExprNode {\n");

            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_AND_EXPR:
            str = Str_New("ASTAndExprNode {\n");
            
            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_EQ_EXPR:
            str = Str_New("ASTEqExprNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("op=");
            APPEND_FREE(str, str2);
            str2 = Str_New(token_type_names[node->as_expr.op]);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New(" }");
            APPEND_FREE(str, str2);
            break;
        case NODE_COMP_EXPR:
            str = Str_New("ASTCompExprNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("op=");
            APPEND_FREE(str, str2);
            str2 = Str_New(token_type_names[node->as_expr.op]);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New(" }");
            APPEND_FREE(str, str2);
            break;
        case NODE_ARITH_EXPR:
            str = Str_New("ASTArithExprNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("op=");
            APPEND_FREE(str, str2);
            str2 = Str_New(token_type_names[node->as_expr.op]);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New(" }");
            APPEND_FREE(str, str2);
            break;
        case NODE_TERM_EXPR:
            str = Str_New("ASTTermExprNode {\n");
            
            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_FACTOR_EXPR:
            str = Str_New("ASTFactorExprNode {\n");
            
            for (size_t i = 0; i < Vec_GetLength(node->as_expr.values); i++) {
                str2 = ASTNode_Spaces(indent + 4);
                APPEND_FREE(str, str2);
                str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, i), indent + 4);
                APPEND_FREE(str, str2);
                str2 = Str_New(i != Vec_GetLength(node->as_dotted_expr.components) - 1 ? ",\n" : "\n");
                APPEND_FREE(str, str2);
            }

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_UNARY_EXPR:
            str = Str_New("ASTUnaryExprNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("op=");
            APPEND_FREE(str, str2);
            str2 = Str_New(token_type_names[node->as_expr.op]);
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(Vec_GetAt(node->as_expr.values, 0), indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
        case NODE_STATEMENT:
            str = Str_New("ASTStatementNode {\n");

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("is_mod_statement=");
            APPEND_FREE(str, str2);
            str2 = Str_New(node->as_statement.is_mod_statement ? "true" : "false");
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("is_return_expr=");
            APPEND_FREE(str, str2);
            str2 = Str_New(node->as_statement.is_return_expr ? "true" : "false");
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("is_local_return=");
            APPEND_FREE(str, str2);
            str2 = Str_New(node->as_statement.is_local_return ? "true" : "false");
            APPEND_FREE(str, str2);
            str2 = Str_New(",\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent + 4);
            APPEND_FREE(str, str2);
            str2 = ASTNode_ToStringIndent(node->as_statement.value, indent + 4);
            APPEND_FREE(str, str2);
            str2 = Str_New("\n");
            APPEND_FREE(str, str2);

            str2 = ASTNode_Spaces(indent);
            APPEND_FREE(str, str2);
            str2 = Str_New("}");
            APPEND_FREE(str, str2);
            break;
    }
    #undef APPEND_FREE

    return str;
}

/**
 * Builds the string representation of an AST node
 * @param[in] node The node to build the string from
 * @returns        The string representation of the node
 */
string * ASTNode_ToString(ast_node_t * node) {
    return ASTNode_ToStringIndent(node, 0);
}
