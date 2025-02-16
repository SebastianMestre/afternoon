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

# Programming in Joy

Since the only type in joy is that of Joy programs, programming in Joy shares a
lot with programming in lambda-calculus, but with an imperative feel.

Before proceeding, I will introduce some notion of types in Joy. Each function
in Joy has some expectations about the state of the stack before it's called,
and it makes some assurances about it after it's done.

For example, `dup` expects an element at the top of the stack, and it ensures
that there will be two values of the same type at the top after it's done. We
can encode this with the type `a -- a a`.

Similarly, the type of `swap` is `a b -- b a` and the type of `pop` is `a --`.



## Booleans

In Joy we write everything in postfix notation because that is how the
evaluation order works, so an if expression will look like this:

    a b condition if

When `condition` is "true", we want this program to reduce to a. When it is
"false", we want it to reduce to b.

To achieve this, we must pick suitable representations for `true`, `false` and
`if`. Here is one such way:

    true  : -- ( a b -- a )
    true  = [ pop ]

    false : -- ( a b -- b )
    false = [ swap pop ]

    if    = eval

In this section I wrote the types of `true` and `false` in as general a way as
possible. The consequence of this is that they have different types. It would be
valid -- and perhaps more faithful to their intent -- to give them a more
specific type:

    type bool = -- (a a -- a)
    true  : bool
    false : bool

By the way, a problem with this representation is that a and b must already be
evaluated. We avoid this by wrapping `a` and `b` with brackets, and make it so
`if` removes the brackets.

    [ a ] [ b ] condition if

where

    true  : bool
    true  = [ pop ]

    false : bool
    false = [ swap pop ]

    if    : (-- a) (-- a) bool -- a
    if    = eval eval


## Pairs

We want to define `make-pair`, `fst` and `snd` such that:

    a b make-pair fst -*-> a
    a b make-pair snd -*-> b

We can represent the pair (a, b) with a program that, when executed, places a
and b on the stack. i.e.:

    type a b pair = -- a b

Then, a possible implementation for `make-pair` is:

    make-pair : a b -- (a b pair)
    make-pair = quote swap quote swap concat

Let's see a step-by-step rewrite:

    [ P ] [ Q ] make-pair
    [ P ] [ Q ] quote swap quote swap concat
    [ P ] [ [ Q ] ] swap quote swap concat
    [ [ Q ] ] [ P ] quote swap concat
    [ [ Q ] ] [ [ P ] ] swap concat
    [ [ P ] ] [ [ Q ] ] concat
    [ [ P ] [ Q ] ]

Then, to get each element we put the elements on the stack and pop the one we
don't want.

    first  : (a b pair) -- a
    first  = eval pop

    second : (a b pair) -- b
    second = eval swap pop

Step-by-step:

    [ [ P ] [ Q ] ] first
    [ [ P ] [ Q ] ] eval pop
    [ P ] [ Q ] pop
    [ P ]

## Stack manipulation

In more traditional concatenative languages (e.g. Forth) there are many more
built-in stack manipulation words. For example: `rot`, which rotates the top
three elements of the stack (`a b c -- b c a`).

This can actually be implemented in Joy.

    rot : a b c -- b c a
    rot = quote swap quote swap concat swap quote concat eval

It looks a bit intimidating, but it's doing the same quote-then-concat trick
that we did with pairs, twice. Here is the step-by-step:

    [ A ] [ B ] [ C ] quote swap quote swap concat swap quote concat eval
    [ A ] [ B ] [ [ C ] ] swap quote swap concat swap quote concat eval
    [ A ] [ [ C ] ] [ B ] quote swap concat swap quote concat eval
    [ A ] [ [ C ] ] [ [ B ] ] swap concat swap quote concat eval
    [ A ] [ [ B ] ] [ [ C ] ] concat swap quote concat eval
    [ A ] [ [ B ] [ C ] ] swap quote concat eval
    [ [ B ] [ C ] ] [ A ] quote concat eval
    [ [ B ] [ C ] ] [ [ A ] ] concat eval
    [ [ B ] [ C ] [ A ] ] eval
    [ B ] [ C ] [ A ]

Another useful stack manipulator is `over`, which copies the second element of
the stack (`a b -- a b a`). There may be more efficient implementations, but one
way to do it is as follows:

    over : a b -- a b a
    over = swap dup rot rot

## Lists

We represent lists as folds. e.g. the list `<x y z>` is represented as a
function that takes function `f` and value `e` and returns `f(x,f(y,f(z,e)))`.

This means that the type of lists will be:

    a list = (a b -- b) b -- b

The implementation is not entirely straightforward, but the most complicated
part of it is just shuffling things around on the stack (to follow the execution
of something like `A B nil cons cons F E fold` and see that it indeed reduces to
something equivalent to `A B E F F`).

    nil : -- (a list)
    nil = [ swap pop ]

    cons : (a list) a -- (a list)
    cons = quote swap quote swap concat [ sink4 rot dup sink4 sink4 eval swap eval ] concat

    fold : (a list) (a b -- b) b -- b
    fold = rot eval

> where `sink4` is a permutation `a b c d -- b a b c`

Then, operations may be defined:

    list-concat : (a list) (a list) -- (a list)
    list-concat = [ cons ] swap fold

## Numbers

There are many ways. One is as lists of `[ ]`.

    zero = nil
    succ = [ ] cons

Then, for example

    add = list-concat
