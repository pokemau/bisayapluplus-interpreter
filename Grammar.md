

<PROGRAM>       ::= 'SUGOD' <STATEMENT>* 'KATAPUSAN'

<STATEMENT>     ::= <IPAKITA> | <VAR-DECL> | <ASSIGNMENT> | <IF-ELSE>

<VAR-DECL>      ::= 'MUGNA' <VAR-TYPE> <DECL-LIST>

<VAR-TYPE>      ::= 'NUMERO'| 'LETRA' | 'TIPIK' | 'TINUOD'
<DECL-LIST>     ::= <DECLARATOR> | <DECLARATOR>, <DECL-LIST>
<DECLARATOR>    ::= <VAR-NAME> | <VAR-NAME> '=' <VALUE>
<VALUE>         ::= <INT> | <CHAR> | <BOOL> | <FLOAT>

<ASSIGNMENT>    ::= <VAR-NAME> '=' <EXPRESSION>

<VAR-NAME>      ::= alphanumeric, can start with '_' or alphabet


<IPAKITA>       ::= 'IPAKITA:' <PRINT-LIST>

<PRINT-LIST>    ::= <PRINT-ITEM> | <PRINT-ITEM> '&' <PRINT-LIST>
<PRINT-ITEM>    ::= <VAR-NAME> | '$' | '[' <ANY-CHAR>']'


<expression>  ::= <equality>
<equality>    ::= <comparison> ( ( "!=" | "==" ) <comparison> )*
<comparison>  ::= <term> ( ( ">" | ">=" | "<" | "<=" ) <term> )*
<term>        ::= <factor> ( ( "-" | "+" ) <factor> )*
<factor>      ::= <unary> ( ( "/" | "*" ) <unary> )*
<unary>       ::= ( "!" | "-" ) <unary> | <primary>
<primary>     ::= NUMBER | STRING | "true" | "false" | "nil"
                    | "(" <expression> ")"
