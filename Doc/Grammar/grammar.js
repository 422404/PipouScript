const NonTerminalLink = (text) => NonTerminal(text, {href: `#${text.replace(/_/g, "-")}`});

// Tokens
const identifier = Diagram(
    Choice(0,
        NonTerminalLink("letter"),
        "'$'",
        "'_'"),
    ZeroOrMore(
        Sequence(
            "'$'",
            "'_'",
            NonTerminalLink("letter"),
            NonTerminalLink("digit")
        )
    )
);

const letter = Diagram("'a'..'z', 'A'..'Z', 0xc0..0xff");

const integer = Diagram(
    Optional("'-'"),
    OneOrMore(Choice(1, "'0'", "'1'", "...", "'9'"))
);

const double = Diagram(
    Optional("'-'"),
    OneOrMore(Choice(1, "'0'", "'1'", "...", "'9'")),
    "'.'",
    OneOrMore(Choice(1, "'0'", "'1'", "...", "'9'")),
    Optional(Sequence("'e'", OneOrMore(Choice(1, "'0'", "'1'", "...", "'9'"))))
);

const string = Diagram(
    "'\"'",
    ZeroOrMore("any ascii char but '\"' and '\\n'"),
    "'\"'",
);

// Grammar
const array = Diagram(
    "'['",
    Optional(Sequence(
        NonTerminalLink("expression"),
        ZeroOrMore(Sequence("','", NonTerminalLink("expression")))
    )),
    "']'"
);

const object_field_name = Diagram(
    Choice(0,
        NonTerminalLink("identifier"),
        NonTerminalLink("object_message_name"))
);

const object_message_name = Diagram(
    "'#'",
    NonTerminalLink("identifier"),
    ZeroOrMore(Sequence("':'", NonTerminalLink("identifier")))
);

const object_field_init = Diagram(
    NonTerminalLink("object_field_name"),
    "':'",
    NonTerminalLink("expression")
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
    identifier.addTo(document.getElementById("identifier-diagram"));
    integer.addTo(document.getElementById("integer-diagram"));
    double.addTo(document.getElementById("double-diagram"));
    string.addTo(document.getElementById("string-diagram"));

    // Grammar
    array.addTo(document.getElementById("array-diagram"));
    object_field_name.addTo(document.getElementById("object-field-name-diagram"));
    object_message_name.addTo(document.getElementById("object-message-name-diagram"));
    object_field_init.addTo(document.getElementById("object-field-init-diagram"));
    function_definition.addTo(document.getElementById("function-definition-diagram"));
    object.addTo(document.getElementById("object-diagram"));
    block.addTo(document.getElementById("block-diagram"));
    block_params.addTo(document.getElementById("block-params-diagram"));
    atom_expression.addTo(document.getElementById("atom-expression-diagram"));
};
