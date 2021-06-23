import json
from prepare_corpus import *
import pandas as pd
from watson_developer_cloud import NaturalLanguageUnderstandingV1
import watson_developer_cloud.natural_language_understanding_v1 as ai
import logging

# This defines the name of the error log file
logging.basicConfig(filename='error.log', level=logging.ERROR)

# If service instance provides API key authentication
# service = NaturalLanguageUnderstandingV1(
#     version='2018-03-16',
#     url='https://gateway.watsonplatform.net/natural-language-understanding/api',
#     iam_apikey='your_apikey')

def getKey(item):
    return int(item["created"])


service = NaturalLanguageUnderstandingV1(
    version='2018-03-16',
    # url is optional, and defaults to the URL below. Use the correct URL for your region.
    url='https://gateway.watsonplatform.net/natural-language-understanding/api',
    username='c2781ef5-373f-4dcb-8ce5-8ac101f00903',
    password='gp0r4wymoCbh')

with open('largeSample.json', 'r') as inputFile:
    comments_dict_temp = json.load(inputFile)
    comments_dict = sorted(comments_dict_temp, key=getKey)

# print("-----------------------------------------")

# for comment in comments_dict:
#  print(getKey(item=comment))

output = []
counter = 0
with open('largeResult.json', 'w') as outfile:
    for comment in comments_dict:
        counter += 1
        comment_prepared = text_prepare(comment['body'])
        comment['body'] = urlRm(comment['body'])
        # (comment_prepared)
        if comment_prepared == "0": continue
        if len(comment['body']) < 4: continue
        try:
            print('in here')
            response = service.analyze(text=comment['body'],
                                       features=ai.Features(
                                           sentiment=ai.SentimentOptions(),
                                           emotion=ai.EmotionOptions(),
                                           keywords=ai.KeywordsOptions(sentiment=True, emotion=True, limit=1))
                                       ).get_result()
            print('passed')
            print(response)
        except Exception as e:
            print("Something went wrong.... Error: ", e)
            logging.error(e)  # This should log the error messages to an error.log file
            continue

        if ('emotion' not in response) or len(response['keywords']) == 0:
            continue
        print('out!')
        thisTuple = {"text": comment['body'],
                     "text_prepared": comment_prepared,
                     "time": comment['created'],
                     "keyword": response['keywords'][0]['text'],
                     "keyword_sentiment_label": response['keywords'][0]['sentiment']['label'],
                     "keyword_sentiment_score": response['keywords'][0]['sentiment']['score'],
                     "emotion_sadness": response['emotion']['document']['emotion']['sadness'],
                     "emotion_joy": response['emotion']['document']['emotion']['joy'],
                     "emotion_fear": response['emotion']['document']['emotion']['fear'],
                     "emotion_disgust": response['emotion']['document']['emotion']['disgust'],
                     "emotion_anger": response['emotion']['document']['emotion']['anger'],
                     "sentiment_score": response['sentiment']['document']['score'],
                     "sentiment_label": response['sentiment']['document']['label'],
                     }
        output.append(thisTuple)

        # dump to json every 3 collections
        if counter % 3 == 0:
            json.dump(output, outfile, indent=2)
            output.clear()
            print('dumped!')

# SHOULD catch final section of output
outfile = open('largeResult.json', 'w')
json.dump(output, outfile, indent=2)
print('final dump!')

# data_topic=pd.read_json('data.json',encoding='utf-8',sep=',')
# print(json.dumps(response, indent=2))

# with open('data.json','w') as outfile:
# json.dump(response,outfile,indent=2)
