#! /usr/bin/python3

import argparse
import sys

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('input', type=argparse.FileType('r'))
	parser.add_argument('-d', '--defs', dest='definitions', type=argparse.FileType('r'))
	parser.add_argument('-o', '--out', dest='output', default=sys.stdout, type=argparse.FileType('w'))

	args = parser.parse_args()

	replacements = {}
	for line in args.definitions:
		pieces = line.strip().split()
		name = pieces[0]
		value = pieces[1:]
		replacements[name] = value

	for line in args.input:
		pieces = line.strip().split()

		result = []

		def append_to_result(word):
			if word not in replacements:
				result.append(word)
			else:
				for piece in replacements[word]:
					append_to_result(piece)

		for piece in pieces:
			append_to_result(piece)

		args.output.writelines(' '.join(result))
		args.output.writelines('\n')

	
if __name__ == '__main__':
	main()
