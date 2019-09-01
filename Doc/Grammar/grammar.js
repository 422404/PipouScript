const NonTerminalLink = (text) => NonTerminal(text, {href: `#${text.replace(/_/g, "-")}`});

// Tokens
const identifier = Diagram(
    Choice(0,
        NonTerminalLink("letter"),
        "'$'",
        "'_'"),
    ZeroOrMore(
        Choice(2,
            "'$'",
            "'_'",
            NonTerminalLink("letter"),
            NonTerminalLink("digit")
        )
    )
);

const letter = Diagram("'a'..'z', 'A'..'Z', 0xc0..0xff");

const digit = Diagram(Choice(1, "'0'", "'1'", "...", "'9'"));

const integer = Diagram(
    Optional("'-'"),
    OneOrMore(NonTerminalLink("digit"))
);

const double = Diagram(
    Optional("'-'"),
    OneOrMore(NonTerminalLink("digit")),
    "'.'",
    OneOrMore(NonTerminalLink("digit")),
    Optional(Sequence("'e'", OneOrMore(NonTerminalLink("digit"))))
);

const string = Diagram(
    "'\"'",
    ZeroOrMore("any ascii char but '\"' and '\\n'"),
    "'\"'",
);

// Grammar

const decl = Diagram(
    NonTerminalLink("identifier"),
    ":=",
    NonTerminalLink("expr"),
    ";"
);

const affect = Diagram(
    NonTerminalLink("dotted_expr"),
    "=",
    NonTerminalLink("expr")
);

const object_field_init = Diagram(
    NonTerminalLink("identifier"),
    "':'",
    NonTerminalLink("expr")
);

const msg_sel = Diagram(
    NonTerminalLink("identifier"),
    Optional(Sequence(
        ":",
        NonTerminalLink("identifier"),
        ZeroOrMore(Sequence(
            NonTerminalLink("identifier"),
            ":",
            NonTerminalLink("identifier")
        ))
    ))
);

const obj_msg_def = Diagram(
    NonTerminalLink("msg_sel"),
    "{",
    ZeroOrMore(NonTerminalLink("statement")),
    "}"
);

const obj_litteral = Diagram(
    "{",
    Optional(Sequence(
        Choice(1,
            NonTerminalLink("obj_field_init"),
            NonTerminalLink("obj_msg_def")
        ),
        ZeroOrMore(Sequence(
            ",",
            Choice(1,
                NonTerminalLink("obj_field_init"),
                NonTerminalLink("obj_msg_def")
            ),
        )),
        Optional(",")
    )),
    "}"
);

/////////////////////////////////////////////////////////////////////////

const array_litteral = Diagram(
    "'['",
    Optional(Sequence(
        NonTerminalLink("expr"),
        ZeroOrMore(Sequence("','", NonTerminalLink("expr"))),
        Optional(",")
    )),
    "']'"
);

const object_field_name = Diagram(
    Choice(0,
        NonTerminalLink("identifier"),
        Sequence("#", NonTerminalLink("identifier"), 
            ZeroOrMore(Sequence(":", NonTerminalLink("identifier")))
        ))
);

const function_definition = Diagram(
    Choice(0,
        NonTerminalLink("identifier"),
        OneOrMore(Sequence(
            NonTerminalLink("identifier"),
            "':'",
            NonTerminalLink("identifier"),
        ))
    ),
    "'{'",
    ZeroOrMore(NonTerminalLink("statement")),
    "'}'"
);

const object = Diagram(
    "'{'",
    Optional(Sequence(
        Choice(0,
            NonTerminalLink("object_field_init"),
            NonTerminalLink("function_definition")
        ),
        OneOrMore(
            "','",
            Choice(0,
                NonTerminalLink("object_field_init"),
                NonTerminalLink("function_definition")
            )
        )
    )),
    "'}'"
);

const block = Diagram(
    "'{'",
    Optional(NonTerminalLink("block_params")),
    ZeroOrMore(NonTerminalLink("statement")),
    "'}'"
);

const block_params = Diagram(
    OneOrMore(NonTerminalLink("identifier")),
    "'|'"
);

const atom_expression = Diagram(
    Optional(Sequence(
        NonTerminalLink("object_field_name"),
        ZeroOrMore(Choice(0,
            Sequence(
                "'['",
                NonTerminalLink("expression"),
                "']'"
            ),
            Sequence(
                "'.'",
                NonTerminalLink("object_field_name")
            )
        ))
    )),
    ZeroOrMore(Sequence(
        NonTerminalLink("identifier"),
        "':'",
        NonTerminalLink("expression")
    ))
);

window.onload = () => {
    // Tokens
    letter.addTo(document.getElementById("letter-diagram"));
    digit.addTo(document.getElementById("digit-diagram"));
    identifier.addTo(document.getElementById("identifier-diagram"));
    integer.addTo(document.getElementById("integer-diagram"));
    double.addTo(document.getElementById("double-diagram"));
    string.addTo(document.getElementById("string-diagram"));

    // Grammar
    decl.addTo(document.getElementById("decl"));
    affect.addTo(document.getElementById("affect"));
    object_field_init.addTo(document.getElementById("obj_field_init"));
    msg_sel.addTo(document.getElementById("msg_sel"));
    obj_msg_def.addTo(document.getElementById("obj_msg_def"));
    obj_litteral.addTo(document.getElementById("obj_litteral"));
};
