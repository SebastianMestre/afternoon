#!/usr/bin/env python3
import random
import math

a = 5 + 3 * random.random()
b = (random.random() - 0.5) * 3

def p(x, a, b):
	return 1 / (1 + math.exp(-(a * x + b)))

l, r = -1, 1
n = 100
samples = [random.uniform(l, r) for _ in range(n)]

def format_points(xs, ys):
	ans = ""
	ans += '['
	sep = ""
	for x, y in zip(xs, ys):
		ans += sep
		sep = ', '
		ans += f'({x:.2f},{y:.2f})'
	ans += ']'
	return ans

true_samples = []
false_samples = []
for x in samples:
	p_x = p(x, a, b)
	if random.random() < p_x:
		true_samples.append(x)
	else:
		false_samples.append(x)

print(format_points(true_samples, [1 for _ in true_samples]))
print(format_points(false_samples, [0 for _ in false_samples]))

def log_likelyhood(a, b):
	ans = 0
	for x in true_samples:
		ans += math.log(p(x, a, b))
	for x in false_samples:
		ans += math.log(1-p(x, a, b))
	return ans


def maximize_convex(l, r, f):
	while r - l > 0.001:
		a = l + (r-l) * 0.3
		b = r - (r-l) * 0.3
		if f(a) < f(b):
			l = a
		else:
			r = b
	return (l + r) / 2

def find_b(a):
	b = maximize_convex(-10, 10, lambda b: log_likelyhood(a, b))
	return b

def find_a_b():
	a = maximize_convex(1, 20, lambda a: log_likelyhood(a, find_b(a)))
	return (a, find_b(a))


print("optimization result:")
print(*find_a_b())

print("ground truth:")
print(a, b)
