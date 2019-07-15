#!/usr/bin/env python3
import sys
import re

def gen_tokens(tokens_file, tokens_h_file, tokens_c_file):
    with open(tokens_h_file, "w") as hf, \
         open(tokens_c_file, "w") as cf, \
         open(tokens_file, "r") as tf:
        tokens = []
        
        for line in tf.readlines():
            match = re.match(r"(?P<name>[a-zA-Z]+)( +(?P<value>'((\\.)|.)'))?", line)
            if match:
                token_name = match.group("name")
                token_value = match.group("value")
                tokens.append((token_name, token_value))
        
        hf.truncate(0)
        hf.write("/**\n"
                 " * @file tokens.h\n"
                 " * (Auto-generated) Tokens declaration\n"
                 " */\n"
                 "#pragma once\n\n")
        if len(tokens):
            for (token_name, token_value) in tokens:
                if token_value:
                    hf.write("#define TOK_" + token_name + (" " + token_value if token_value else "") + "\n")
            
            hf.write("\ntypedef enum {\n")
            for (token_name, _) in tokens:
                hf.write("    TOKTYPE_" + token_name + ",\n")
            
            hf.write("    TOKENS_NUMBER,\n"
                     "    TOKTYPE_NOTTOKEN\n"
                     "} token_type_t;\n\n"
                     "extern char * token_type_names[];\n")
            
            cf.write("/**\n"
                     " * @file tokens.c\n"
                     " * (Auto-generated) Tokens declaration\n"
                     " */\n\n"
                     "char * token_type_names[] = {\n")
            for (token_name, _) in tokens:
                cf.write("    \"TOKTYPE_" + token_name + "\",\n")
            cf.write("};\n")

if __name__ == "__main__":
    gen_tokens(sys.argv[1], sys.argv[2], sys.argv[3])
