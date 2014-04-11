#!/usr/bin/env python
import sys
import heapq
from collections import defaultdict
from itertools import izip, islice

TOPN = 250
INPUT_PATH = sys.argv[1]

bigram_counts = defaultdict(int)
tokenized_data = open(INPUT_PATH, 'r').read().split()
num_tokens = len(tokenized_data)

for bigram in izip(islice(tokenized_data, 0, num_tokens - 1),
                   islice(tokenized_data, 1, num_tokens)):
    bigram_counts[bigram] += 1

for count, bigram in heapq.nlargest(TOPN, ((v,k) for k,v in bigram_counts.iteritems())):
    print bigram[0], bigram[1], count
