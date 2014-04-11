#!/usr/bin/env python
import sys
from collections import Counter
from itertools import izip, islice

TOPN = 250
INPUT_PATH = sys.argv[1]

tokenized_data = open(INPUT_PATH, 'r').read().split()
num_tokens = len(tokenized_data)
bigram_counts = Counter(izip(islice(tokenized_data, 0, num_tokens - 1),
                             islice(tokenized_data, 1, num_tokens)))

for bigram, count in bigram_counts.most_common(TOPN):
    print bigram[0], bigram[1], count
