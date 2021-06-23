import json
from prepare_corpus import *
import pandas as pd
import queue as q
from collections import Counter

with open('largeKey.json','r') as inputFile:
	comments_dict=json.load(inputFile)

with open('timeLargeResult.json','r') as inputFile:
	comments_dict_all=json.load(inputFile)


output=[]

for comment in comments_dict:
	for comment_all in comments_dict_all:
		if comment['Key']==comment_all['keyword']:
			tempTuple={'Key':comment['Key'],
					   'Occurrence':comment['Value'],
					   'Sentiment_score':comment_all['keyword_sentiment_score'],
					   'Time Created':comment_all['time'],
					   'Original Text':comment_all['text']
			}
			output.append(tempTuple)


with open('largeKeyDetail.json','w') as outfile:
	json.dump(output,outfile,indent=2)
