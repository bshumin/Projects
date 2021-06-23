import json
from prepare_corpus import *
import pandas as pd
import queue as q
from collections import Counter

with open('largeResult.json','r') as inputFile:
  comments_dict=json.load(inputFile)

print(comments_dict[1]['keyword'])
print(len(comments_dict))

keywordList=['' for i in range(len(comments_dict))]

for index, comments in enumerate(comments_dict):
	keywordList[index]=comments['keyword']

print('')
print(keywordList)

keywordRecord=[]
count=Counter(keywordList)
#countQ=Counter()

for keys in keywordList:
	if count[keys]>2:
		tempTuple={"Key":keys, "Value": count[keys]}

		if tempTuple not in keywordRecord:
			keywordRecord.append(tempTuple)
		#keywordList=[x for x in keywordList if x!=keys]

print('')
print(keywordRecord)

with open('largeKey.json','w') as outfile:
  json.dump(keywordRecord,outfile,indent=2)


