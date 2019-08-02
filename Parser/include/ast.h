/**
 * @file ast.h
 * Abstract Syntax Tree implementation
 */
#pragma once
#include <stdbool.h>
#include "vector.h"
#include "tokens.h"

typedef enum {
    DECL,
    AFFECT,
    OBJ_FIELD_NAME,
    OBJ_FIELD_INIT,
    MSG_SEL,
    OBJ_MSG_DEF,
    OBJ_LITTERAL,
    ARRAY_LITTERAL,
    BLOCK,
    ARRAY_ACCESS,
    DOTTED_EXPR,
    MSG_PASS_EXPR,
    OR_EXPR,
    AND_EXPR,
    COMP_EXPR,
    ARITH_EXPR,
    TERM_EXPR,
    FACTOR_EXPR,
    UNARY_EXPR,
    ATOM_EXPR,
    STATEMENT
} ast_node_type_t;

struct ast_node_s;
typedef struct ast_node_s ast_node_t;

typedef struct ast_identifier_s {
    char * value;
} ast_identifier_t;

typedef struct ast_string_s {
    char * value;
} ast_string_t;

typedef struct ast_int_s {
    int value;
} ast_int_t;

typedef struct ast_double_s {
    double value;
} ast_double_t;

typedef struct ast_decl_s {
    ast_node_t * lval;
    ast_node_t * rval;
} ast_decl_t;

typedef struct ast_affect_s {
    struct {
        ast_node_t * name_base;
        ast_node_t * field_name;
    } lval;
    ast_node_t * rval;
} ast_affect_t;

typedef struct ast_obj_field_init_s {
    ast_node_t * value;
} ast_obj_field_init_t;

typedef struct ast_msg_sel_s {
    /** vector_t<ast_identifier_t *> */
    vector_t * ident_list;
} ast_msg_sel_t;

typedef struct ast_obj_msg_def_s {
    ast_node_t * selector;
    /** vector_t<ast_statement *> */
    vector_t * statements;
} ast_obj_msg_def_t;

typedef struct ast_obj_litteral_s {
    /** vector_t<ast_obj_field_init * | ast_obj_msg_def *> */
    vector_t * obj_fields;
} ast_obj_litteral_t;

typedef struct ast_obj_field_name_s {
    bool is_msg_name;
    /** vector_t<ast_identifier_t *> */
    vector_t * components;
} ast_obj_field_name_t;

typedef struct ast_array_litteral_s {
    /** vector_t<ast_expr_t *> */
    vector_t * items;
} ast_array_litteral_t;

typedef struct ast_block_s {
    /** vector_t<ast_identifier_t *> */
    vector_t * params;
    /** vector_t<ast_statement *> */
    vector_t * statements;
} ast_block_t;

typedef struct ast_array_access_s {
    ast_node_t * index_expr;
} ast_array_access_t;

typedef struct ast_dotted_expr_s {
    /** vector_t<ast_array_access_t * | ast_identifier_t *> */
    vector_t * components;
} ast_dotted_expr_t;

typedef struct ast_msg_pass_expr_s {
    /** vector_t<ast_atom_expr_t * | ast_identifier_t *> */
    vector_t * components;
} ast_msg_pass_expr_t;

/**
 * Represents expr, or_expr, and_expr, comp_expr, arith_expr, term_expr,
 * factor_expr, unary_expr and atom_expr
 */
typedef struct ast_expr_s {
    /** vector_t<ast_expr_t *> */
    vector_t * values;
    /**
     * arith_expr, comp_expr and unary_expr need more info 
     * on the operator used
     */
    token_type_t op;
} ast_expr_t;

/**
 * Represents statement and mod_statement
 */
typedef struct ast_statement_s {
    bool is_mod_statement;
    bool is_return_expr;
    bool is_local_return;
    ast_node_t * value;
} ast_statement_t;

struct ast_node_s {
    ast_node_type_t type;
    // loc_t loc;
    union {
        ast_identifier_t     as_ident;
        ast_string_t         as_string;
        ast_int_t            as_int;
        ast_double_t         as_double;
        ast_decl_t           as_decl;
        ast_affect_t         as_affect;
        ast_obj_field_init_t as_obj_field_init;
        ast_msg_sel_t        as_msg_sel;
        ast_obj_msg_def_t    as_obj_msg_def;
        ast_obj_litteral_t   as_obj_litteral;
        ast_obj_field_name_t as_obj_field_name;
        ast_array_litteral_t as_array_litteral;
        ast_block_t          as_block;
        ast_array_access_t   as_array_access;
        ast_dotted_expr_t    as_dotted_expr;
        ast_msg_pass_expr_t  as_msg_pass_expr;
        ast_expr_t           as_expr;
        ast_statement_t      as_statement;
    };
};

/**
 * Allocates a new node
 * @param type Node type
 * @returns    A pointer to the newly allocated node
 */
ast_node_t * ASTNode_New(ast_node_type_t type);

/**
 * Frees a node and its children if any
 * @param[int] node The node to free
 */
void ASTNode_Free(ast_node_t * node);
