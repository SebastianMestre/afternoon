# 2048

Aims to answer the question:

> What is the shortest possible game of 2048, if you have perfect luck and
> perfect strategy?

Just a brute force DFS written in C and a runner program in bash.

The C program explores about 65 million boards per second on my machine.

Simple lower bound: 512, because we need the tiles to add up to 2048, and the
highest tile that spawns naturally has a value of 4, meaning 2048/4 is the least
possible number of spawns to reach 2048.

A better lower bound: After the 512th 4-tile spawns, the player needs to play 10
turns to get it merged into an 8-tile, then 16-tile, and so on, all the way up
to a 2048-tile. The first of these 10 turns is turn 512, and the last is 521.
This would imply that 521 is a lower bound.

The best upper bound found so far is 523 steps. Found after exploring 17.03
billion states in 298.47s (just under 5 minutes). The final board is:

```
[    4|   16|   16|     ]
[    4| 2048|     |     ]
[    4|     |     |     ]
[     |     |     |     ]
```

> In the theoretical lower bound, the remaining tiles (ignoring the 2048-tile)
> would add up to `36 = 9*4`, and in this case the remaining tiles add up to
> `44 = 11*4`.
