# Joy

A Joy interpreter written in C.

Joy is a little language that can be conceptualized as a DSL for operating on a
stack. For example, the `pop` operator removes the top element of the stack.

           pop
    a b c ----> a b

The basic stack manipulation primitives in Joy are `pop`, `swap` and `dup`.
`swap` exchanges the two elements on the top of the stack. `dup` pushes another
copy of the element at the top of the stack.

In Joy, operators are written one after the other and evaluated in that order.
For example the program `pop pop` removes two elements from the top of the
stack.

Joy also supports manipulating programs as data: the `[ _ ]` operator pushes the
program between brackets onto the stack, `eval` takes the program that's at the
top of the stack and executes it. In fact, the only data type in Joy is the type
of Joy programs.

For example, these programs are equivalent:

    [ [] dup ] eval eval
    [] dup eval
    [] [] eval
    []

You may have noticed that consecutive lines differ by local modifications, and
wonder if there is a rewrite-based semantics for Joy. This is indeed the case,
and here are some (but not all) of the rules:

    [ P ] eval       -> P
    [ P ] dup        -> [ P ] [ P ]
	[ P ] pop        -> -
	[ P ] [ Q ] swap -> [ Q ] [ P ]

Using these we can derive these multistep rules:

    [ P ] dup pop         -*-> [ P ]
    [ P ] [ Q ] swap swap -*-> [ P ] [ Q ]

There are a few more operators that are better understood by looking at the
rewrite semantics: `quote` and `concat`.

    [ P ] quote        -> [ [ P ] ]
	[ P ] [ Q ] concat -> [ P Q ]

The stack-based semantics of Joy correspond to repeatedly performing the
leftmost rewrite that is not inside brackets.

Adopting this convention we have that, during evaluation, the program always
starts with a sequence of programs inside brackets, which corresponds exactly to
the state of the stack during execution.

    [] dup dup [ [] [] ] swap eval eval
    [] [] dup [ [] [] ] swap eval eval
    [] [] [] [ [] [] ] swap eval eval
    [] [] [ [] [] ] [] eval eval
    [] [] [ [] [] ] eval
    [] [] [] []

Another way to think about Joy is that programs are functions from stacks to
stacks. From this perspective we can see that program concatenation (assumming
well-paired brackets) is equivalent to function composition. Thus Joy is called
a concatenative language.

We have now seen all of Joy. Here is a quick reference:

- `pop`: removes a value from the top of the stack
- `dup`: pushes a copy of the value at the top of the stack
- `swap`: exchanges the two values at the top of the stack
- `[ _ ]`: pushes the program between brackets to the top of the stack
- `eval`: executes the program that is at the top of the stack
- `concat`: concatenates the two programs that are at the top of the stack
- `quote`: replaces the top of the stack with a program that produces its value

I think usually Joy implementations also let you assign programs to names, but I
have not implemented this feature...
