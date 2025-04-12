# Inline Caches

Hidden classes is a mechanism by which Javascript interpreters optimize property
access on objects.

Morally, a javascript object is much like a hash map, so it would be reasonable
to implement them as such. However, this is not a desirable implementation, as
it wastes space (hash maps must be sparse to support fast insertions), and
is relatively expensive time-wise (hashing, mod, collision handling).

To address the space problem, we take advantage of the following: there are
usually many objects with the same fields. So we can represent an object as a
flat array of their fields and have a single mapping of key to position for all
objects with identical sets of keys.

We call such a mapping a "shape" or a "hidden class".

This will likely be faster due to a smaller memory footprint (and fewer
allocations if we were using a node-based hash map) but we still have a lot of
expensive work on each lookup. To address this, we observe that on a given code
location that performs field access, it will usually always run on objects with
the same hidden class.

To take advantage of this, we extend each such code location with an "inline
cache", that remembers the field position for a given hidden class, and can be
used if the code is re-run with the same hidden class, avoiding a hash map
lookup. The cache needs to be invalidated if the code is run with a different
hidden class.

Since most field accesses should be cached, we can afford to represent shapes as
linked lists instead of hash maps.

## Performance

I set up a really simple mirobenchmark.

First, we create two objects with the same 20 keys each. Then we repeatedly
replace the value assigned to the first key millions of times.

This is an extreme worst case scenario for the "no cache" solution, so we can
observe some extreme speedups:

```sh
$ cc -O3 *.c -o hidden-class
$ ./hidden-class
elapsed: 1267.08ms

$ cc -O3 *.c -o hidden-class -DUSE_CACHE
$ ./hidden-class
elapsed: 161.27ms
```

Link-time optimization shifts things around but it's still a huge win:

```
$ cc -O3    *.c   -o main -flto
$ ./main
elapsed: 450.26ms

$ cc -O3    *.c   -o main -flto -DUSE_CACHE
$ ./main
elapsed: 138.92ms
```

I haven't looked at what causes the extreme (1250ms -> 450ms) speedup on the
non-cached case. I am guessing LTO is enough for the compiler to 'see through'
most of the lookup logic and just hard-code the offsets? Although in that case
it should be even closer to the cached case...
