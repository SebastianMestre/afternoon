
# Logistic regression

Consider the family of random variables $V_x$, where $x$ is a real number.

The domain of $V_x$ is $\{0,1\}$.

The function $p(x) = P(V_x = 1)$ is non-decreasing.

Given a set of samples $\{x_i : i = 1,\dots,n\}$ where $V_{x_i}$ was found to be
`1` and a set $\{y_i : i = 1,\dots,m\}$ where $V_{y_i}$ was found to be `0`. We
want to classify new values of $x$ into `0` and `1` classes.

We could do a whole bunch of ad-hoc things to conjure up a threshold, but it
turns out that the standard solution is to estimate the function $p(x)$ then
using it to check if $p(x) < \frac{1}{2}$.

So let's start by making some assumptions about $p$:

- non-decreasing
- strictly bound between $0$ and $1$
- $p \in C^{\infty}$
- goes to 0 as $x$ goes to $-\inf$
- goes to 1 as $x$ goes to $+\inf$

This is not enough to fully constraint our options, for example $tan^{-1}$
works, but let's settle for a rescaling of the logistic function
$f(x) = (1 + \exp(-x))^{-1}$. There is a good reason for this.

So in particular, we are looking at functions of the form $g(x) = f(ax+b)$ where
$a \in \mathbb{R}^{+}, b \in \mathbb{R}$.

One way to approach this is by computing the probability the probability of
observing the data for each posible function, and taking the function that
yields the highest probability.
So we want to solve the optimization problem:

$$\max_{(a,b)} \space \prod_{i=1}^n g(x_i) \times \prod_{i=1}^m (1-g(y_i)) $$

This should raise a few eyebrows, the quantity we've defined should, a priori,
not tell us anything about how "good" the function is at describing the data or
the underlying distribution. Indeed, this number is not at all the probability
that the function we picked is right. (what would that even mean?)

What we have defined is called a likelyhood. And what we want to find is called
the maximum-likelihood estimator.

Expanding out:

$$\max_{(a,b)} \space \prod_{i=1}^n (1 + \exp(-a x_i - b))^{-1} \times \prod_{i=1}^m (1-(1 + \exp(-ay_i - b))^{-1}) $$

Taking the logarithm we get an equivalent problem, because $\log$ is increasing:

$$\max_{(a,b)} \space \sum_{i=1}^n \log((1 + \exp(-a x_i - b))^{-1}) + \sum_{i=1}^m \log(1-(1 + \exp(-ay_i - b))^{-1}) $$

This is a bit ugly, but the functions inside the sums are convex on both $a$ and
$b$, so it's easy to optimize numerically, which is why we used this family of
functions :). (and probably more reasons, but I am not privy to them)

For example, we can use a ternary search.

## Conclusions

A cool way to think about classification problems is through the lens of
maximum-likelihood estimation. A similar idea exists for regression problems. In
fact least-squares curve fitting can be framed as another instance of
maximum-likelihoof estimation.

Convex functions are powerful cool and easy to optimize if low-dimensional using
nested ternary search.
