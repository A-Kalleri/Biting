# Bitter Grammar

## Literals

| Syntax | Meaning |
|:-------|:--------|
| `0` | Bit value `0` |
| `1` | Bit value `1` |

## Operators

| Operator | Operation |
|:---------|:----------|
| `\|` | OR |
| `&` | AND |
| `^` | XOR |
| `~` / `!` | NOT |
| `>` | Feed |
| `<` | Show |
| `.` | Reset LHS |
| `?` | Read |

## Registers

### LHS

**LHS** (Left-Hand-Side) is an internal register that accumulates the results of operations.

### RP

**RP** (Print register) written `*p` stores a value that can be printed using the `<` Show operator.

The `<` operator prioritizes RP over LHS.

If RP contains a value, `<` prints RP. If RP is empty, `<` prints LHS, and if both are empty, `<` prints nothing.

### RR

**RR** (Read register) written `*r` stores a value that is read from the standard input.

It only stores a bit value `1` or `0`

## Variables

```text
x<numbers>
```

Variables are identified by the 'x' prefix followed by one or more digits.

Variable Rules:

- Variable names consist of the `x` prefix followed by numbers.
- Variables indices cannot exceed `255`.
- Valid varable indices range from 0 to 255.

The variables are internally mapped to an array of bits, and all the variables are initialized to empty at start.

The size of the array currently is `0 - 255`.

## NOT Operator

The `~` or the `!` operator negates the value inside the LHS register.

```text
1 | 0 !
```

`1 | 0` produces `1` and stores it in LHS. The `!` operator then negates the value to `0`, so LHS has the value `0` at the end.

## Feed Operator

The `>` Feed operator feeds the value of the LHS register into a variable or register.

For example:

```text
1 | 0 > x0
```

`1 | 0` produces `1`.
The result is then stored in `x0`.

## Show Operator

The `<` Show operator prints a value to standard output.

There are two ways to use the Show operator.

### Showing RP

A value can first be stored in the RP register:

```text
1 | 0 > *p <
```

This stores the result of `1 | 0` in RP and then prints RP.

### Showing LHS

The result can also be printed directly:

```text
1 | 0 <
```

Since RP is empty, the `<` operator prints the value currently stored in LHS.

>[!NOTE]
>The `<` operator always prioritizes RP.

The priority is:
```text
RP -> LHS
```
## Reset LHS

The `.` operator resets the LHS register to an empty state.

Example:

```text
0 | 0 < . 1 <
```

`0 | 0` produces `0`, and stores it in LHS.

The `<` operator prints the LHS value.

The `.` operator then resets LHS.

Then the value `1` is loaded into LHS and printed.

Therefore, the output is `01`.

>[!IMPORTANT]
>LHS must be empty before a new value is loaded into it.
>```text
>1 0
>```
>would result in an error if 1 is already stored in LHS and has not been reset.

## Read Operator

The `?` operator is used to read a bit from the standard input into the RR.

```text
? *r > x0
```

`?` reads the user input and stores it into RR.

The value from RR is then fed into x0.

x0 now contains the user input.