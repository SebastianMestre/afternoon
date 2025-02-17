# Replacer

Takes definition files and replaces every instance of a defined name with its
definition. Definitions may use other names that have a defined substitution and
these will also be taken into account. (i.e. substitutions are applied
recursively)

    usage: replacer.py [-h] [-d DEFINITIONS] [-o OUTPUT] input
    
    positional arguments:
      input
    
    options:
      -h, --help            show this help message and exit
      -d DEFINITIONS, --defs DEFINITIONS
      -o OUTPUT, --out OUTPUT

definition files look like:

    name1 token11 token12 token13 ...
    name2 token21 token22 token23 ...

Meaning `name1` is to be replaced with `token11 token12 token13`. (and similar
for `token2`)

The usecase for this is writing Joy programs with custom words that get
"desugared" before being passed to the interpreter. For example:

    ./replacer.py example/program.joy -d example/defs -o example/output.joy
