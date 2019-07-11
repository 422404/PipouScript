#!/usr/bin/env python3
import sys
import re

def gen_tokens(tokens_file, token_h_file):
    with open(token_h_file, "w") as hf, \
         open(tokens_file, "r") as tf:
        tokens = []
        
        for line in tf.readlines():
            match = re.match(r"(?P<name>[a-zA-Z]+)( +(?P<value>'(.|\\.)'))?", line)
            if match:
                token_name = match.group("name")
                token_value = match.group("value")
                tokens.append((token_name, token_value))
        
        hf.truncate(0)
        hf.write("#pragma once\n\n")
        if len(tokens):
            for (token_name, token_value) in tokens:
                if token_value:
                    hf.write("#define TOK_" + token_name + (" " + token_value if token_value else "") + "\n")
            hf.write("\ntypedef enum {\n")
            for (token_name, _) in tokens:
                hf.write("    TOKTYPE_" + token_name + ",\n")
            hf.write("    TOKENS_NUMBER\n} token_type_t;\n")

if __name__ == "__main__":
    gen_tokens(sys.argv[1], sys.argv[2])
