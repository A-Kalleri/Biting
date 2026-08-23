# Bitter Execution

Bitter executes a program sequentially from the beginning of the source to the end.

Each operation is evaluated in order, and its result is stored in the LHS register.

## Registers

Bitter currently has 4 internal registers:

### LHS

The **LHS** (Left-Hand-Side) register is a user-controlled register that stores the result of the current operation.

Operators accumulate their results in LHS.

LHS can contain:

- `0`
- `1`
- Empty

When writing a line `1 | 0` the program loads `1` to LHS automatically.

When `1` a single operand, the value is loaded into LHS.

A value cannot be loaded into LHS while it already contains a value.

The `.` operator is used to reset LHS.

### RP

The **RP** (Read/Print) register stores a value that can be printed using the `<` operator.

RP can contain:

- `0`
- `1`
- Empty

### RHS

The **RHS** (Right-Hand-Side) register stores the other value of the binary operation.

It cannot be manipulated by the user; it can only be loaded, and after every operation, it is emptied.

RHS can contain:

- `0`
- `1`
- Empty

### OP

The **OP** (Operation) register stores the opcode of the operation.

It is an internal register only for the program and is emptied after an operation.

It can store any opcode of the boolean operation supported by the language.

## Variables

Bitter provides 256 variables:

```text
x0 - x255
```

Each variable can contain:

- `0`
- `1`
- Empty

All variables are initialized to empty when execution begins.

## Execution Flow

An operation is evaluated and its result is stored in LHS.

For example:

```text
1 | 0
```

First the value `1` is loaded into LHS, then the value `|` is loaded into OP and finally the value `0` is loaded into RHS preparing for the operation.
The result is then written back to LHS and the other registers, RHS and OP are emptied.

Finally, LHS has the value `1`.

## Grammar

The following grammar is written in Backus-Naur form (BNF).

### Program

```text
<program> ::= <statement>*

<statement> ::= <expression>
              | <feed>
              | <read>
              | <reset>
```

### Expression

```text
<expression> ::= <operand>
               | <operand> <binary-operator> <operand>
               | <unary-operator>
```

### Operands

```text
<operand> ::= <literal>
            | <variable>
            | <register>
```

### Binary Operators

```text
<binary-operator> ::= "|"
                    | "&"
                    | "^"
```

### Unary Operators

```text
<unary-operator> ::= "~"
                   | "!"
```

### Feed

```text
<feed> ::= ">" <destination>

<destination> ::= <variable>
                | <register>
```

### Read

```text
<read> ::= "<"
```

### Reset

```text
<reset> ::= "."
```

### Register

```text
<register> ::= "rp"
```

### Variables

```text
<variable> ::= "x" <digit>+

<digit> ::= "0"
          | "1"
          | "2"
          | "3"
          | "4"
          | "5"
          | "6"
          | "7"
          | "8"
          | "9"
```

### Literals

```text
<literal> ::= "0"
            | "1"
```
