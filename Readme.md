# Bisaya++ Interpreter

Bisaya++ is a simple, interpreted programming language inspired by Cebuano/Bisaya terminology. It supports variables, loops, conditionals, and basic arithmetic operations, designed for educational purposes and scripting.

## Features
- **Variable Declarations**: Define variables with types (`NUMERO`, `LETRA`, `TIPIK`, `TINUOD`).
- **Control Structures**: For-loops (`ALANG SA`), if-else (`KUNG`, `KUNG_DILI`, `KUNG_WALA`).
- **Printing**: Output values and newlines (`IPAKITA`).
- **Logical Operations**: Boolean operations with `UG` (and) and `O` (or).
- **Arithmetic**: Basic operations (`+`, `-`, `*`, `/`, `%`).

## Language Specification

### Syntax

- **Program Structure**: A program starts with `SUGOD` and ends with `KATAPUSAN`.
- **Variable Declaration**: Use `MUGNA <type> <name>` to declare variables.
  Types: <br>
  - `NUMERO` (integer), <br>
  - `LETRA` (char), <br>
  - `TIPIK` (double), <br>
  - `TINUOD` (boolean).
- **For-Loop**: `ALANG SA (<init>, <condition>, <update>) PUNDOK { <body> }`
  - `init`: Assignment (e.g., `ctr = 1`).
  - `condition`: Comparison (e.g., `ctr <= 5`).
  - `update`: Assignment (e.g., `ctr = ctr + 1`).
- **Printing**: `IPAKITA: <expr> & <expr> & ... & $` prints expressions, with `$` for newline.
- **Assignments**: `<var> = <expr>` updates a variable.
- **Expressions**:
  - Literals: Numbers (`123`, `3.14`), chars (`'a'`), booleans (`"OO"`, `"DILI"`).
  - Operators: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `<`, `<=`, `>`, `>=`, `UG` (and), `O` (or).
  - Unary: `-` (negation), `DILI` (not).
- **Blocks**: `PUNDOK { <statements> }` groups statements.

### Example

```plaintext
SUGOD
    -- This code prints from 1 to 5
    MUGNA NUMERO ctr
    ALANG SA (ctr = 1, ctr <= 5, ctr++)
    PUNDOK {
        IPAKITA: ctr & $
    }
KATAPUSAN
```
