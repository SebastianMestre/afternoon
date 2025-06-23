# 2048

Aims to answer the question:

> what is the shortest possible game of 2048, if you have perfect luck and
> perfect strategy?

Just a brute force DFS written in C and a runner program in bash.

The C program explores about 16.5 million boards per second on my machine.

Theoretical lower bound: 512, because we need the tiles to add up to 2048, and
the highest tile that spawns naturally has a value of 4, meaning 2048/4 is the
least possible number of spawns to reach 2048.

Best upper bound found so far is 525 steps. With the final board being this:

```
[    4|     |     |     ]
[   16|     |     |     ]
[    8|     |     |     ]
[    4|   16| 2048|     ]
```

