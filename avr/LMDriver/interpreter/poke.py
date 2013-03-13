#!/usr/bin/python

import ply.lex as lex
import ply.yacc as yacc

data="""
sconst A 't4abcd'
#sconst B 'one two three'
#ivar a 100
#alabel:
    semit A
#    jz alabel
    !
"""

reserved = {
    'sconst': 'SCONST',
    'ivar'  : 'IVAR',
    'svar'  : 'SVAR',
    'jz'    : 'JZ',
    'jump'  : 'JUMP',
    'iemit' : 'IEMIT',
    'semit' : 'SEMIT'
}

# List of token names.   This is always required
tokens = [
        'NUMBER',
        'PLUS',
        'MINUS',
        'EQUALS',
        'SQUOTE',
        'DQUOTE',
        'STRING',
        'HEXNUMBER',
        'COLON',
        'LABEL',
        'IDENTIFIER',
        'END',
        'COMMENT'
        ] + list(reserved.values())
# ------------------------------------------------------------------------------------
# Regular expression rules for simple tokens
t_PLUS      = '\+'
t_MINUS     = '-'
t_SQUOTE    = "'"
t_DQUOTE    = '"'
t_EQUALS    = '='
t_COLON     = ':'
t_END       = '!'

# A regular expression rule with some action code

def t_COMMENT(t):
    r'\#.*'
    pass

def t_IDENTIFIER(t):
    r'[a-zA-Z_]+[0-9a-zA-Z_]+'
    return t

def t_NUMBER(t):
    r'\d+'
    t.value = int(t.value)    
    return t

def t_HEXNUMBER(t):
    r'0x[0-9a-fA-F]+'
    t.value = int(t.value, 16)
    return t

def t_STRING(t):
    r'[0-9a-zA-Z_ ]+'
    return t

def t_LABEL(t):
    r'[0-9a-zA-Z_]+:'
    return t

# Define a rule so we can track line numbers
def t_newline(t):
    r'\n+'
    t.lexer.lineno += len(t.value)

# A string containing ignored characters (spaces and tabs)
t_ignore  = ' \t'

# Error handling rule
def t_error(t):
    print "Illegal character '%s'" % t.value[0]
    t.lexer.skip(1)

# ------------------------------------------------------------------------------------
#
# Parser
#
# ------------------------------------------------------------------------------------
def p_statement(p):
    '''statement : STRING_CONST
       | INT_VARIABLE
       | STRING_VARIABLE
       | INCREMENT
       | DECREMENT
       | JUMPZERO
       | SIMPLEJUMP
       | STRING_EMIT
       | INT_EMIT
       | COMMENT
       '''
    print(t[1])

def p_expression_string_const(p):
    'STRING_CONST : SCONST IDENTIFIER SQUOTE STRING SQUOTE'
    p[0] = p[1:]

def p_expression_int_variable(p):
    'INT_VARIABLE : IVAR IDENTIFIER NUMBER'
    pass

def p_expression_string_variable(p):
    'STRING_VARIABLE : SVAR IDENTIFIER SQUOTE STRING SQUOTE'
    pass

def p_expression_increment(p):
    'INCREMENT : PLUS IDENTIFIER'
    pass

def p_expression_decrement(p):
    'DECREMENT : MINUS IDENTIFIER'
    pass

def p_expression_jumpzero(p):
    'JUMPZERO : JZ IDENTIFIER'
    pass

def p_expression_simplejump(p):
    'SIMPLEJUMP : JUMP IDENTIFIER'
    pass

def p_expression_string_emit(p):
    'STRING_EMIT : SEMIT IDENTIFIER'
    pass

def p_expression_int_emit(p):
    'INT_EMIT : IEMIT IDENTIFIER'
    pass


def p_error(t):
        print("Syntax error at '%s'" % t.value)

#def p_expression_sconst(p):
#    'STRING_CONST : SCONST IDENTIFIER SQUOTE STRING SQUOTE'
#    p[0] = p[1:]
#
#def p_expression_ivar(p):
#    pass
#
#def p_expression_svar(p):
#    pass
#
#def p_expression_inc(p):
#    pass
#
#def p_expression_dec (p):
#    pass
#
#def p_expression_jz(p):
#    pass
#
#def p_expression_jump(p):
#    pass
#
#def p_expression_semit(p):
#    pass
#
#def p_expression_iemit(p):
#    pass

#
#def p_expression_jz(p):
#    'expression : JZ IDENTIFIER'
#    p[0] = p[1,2]
#
#def p_expression_ivar(p):
#    '''expression : IVAR IDENTIFIER NUMBER
#       | IVAR IDENTIFIER HEXNUMBER'''
#    p[0] = p[1,3]
#
#def p_expression_svar(p):
#    'expression : SVAR IDENTIFIER STRING'
#    p[0] = p[1,3]
#
#def p_expression_iemit(p):
#    'expression : IEMIT IDENTIFIER'
#    p[0] = p[1,2]
#
#def p_expression_semit(p):
#    'expression : SEMIT IDENTIFIER'
#    p[0] = p[1,2]



# Build the lexer
lexer = lex.lex()

# Give the lexer some input
#lexer.input(data)

yacc.yacc()
yacc.parse(data)

# Tokenize
#while True:
#    tok = lexer.token()
#    if not tok:
#        break      # No more input
#    print tok

