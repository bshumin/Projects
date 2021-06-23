from datetime import datetime
import json
from prepare_corpus import *
import pandas as pd


with open('largeResult.json','r') as inputfile:
	comment_dict=json.load(inputfile)

output=[]
count_total=0
emot_joy=0
for comment in comment_dict:
	count_total+=1
	emot_joy+=comment['emotion_joy']
	ts=int(comment['time'])
	time=datetime.utcfromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
	comment['time']=time
	output.append(comment)

emot_joy_avg=emot_joy/count_total

emot_joy=0
for comment in comment_dict:
	emot_joy+=(emot_joy_avg-float(comment['emotion_joy']))**2

emot_joy_std=emot_joy/count_total

print("First count: "+str(count_total) + " std: "+str(emot_joy_std))

with open('timeLargeResult.json','w') as outfile:
	json.dump(output,outfile,indent=2)

his=[]
count=0
count_cur=0
year=0
month=0
day=0

for comment in output:
	count_cur+=1
	separate = comment['time'].split(' ')
	separate = separate[0].split('-')
	year_cur=separate[0]
	month_cur=separate[1]
	day_cur=separate[2]
	cur_time=(int(year_cur)-2000)*365+int(month_cur)*30+int(day_cur)

	if count==0:
		separate = comment['time'].split(' ')
		separate = separate[0].split('-')
		year=separate[0]
		month=separate[1]
		day=separate[2]
		time=(int(year)-2000)*365+int(month)*30+int(day)

	if float(comment['sentiment_score'])>0:
		count+=1

	if cur_time-time>6:
		tempTuple={'Sentiment':'Positive',
				   'Time:':str(year)+'-'+str(month)+'-'+str(day)+' to '+str(year_cur)+'-'+str(month_cur)+'-'+str(day_cur),
				   'Count':count,
				   'Total Size Over Time':count_cur,
				   'Ratio':count/count_cur
		}
		his.append(tempTuple)
		count=0
		count_cur=0

count=0
count_total=0
for comment in output:
	count_total+=1
	count_cur+=1
	separate = comment['time'].split(' ')
	separate = separate[0].split('-')
	year_cur=separate[0]
	month_cur=separate[1]
	day_cur=separate[2]
	cur_time=(int(year_cur)-2000)*365+int(month_cur)*30+int(day_cur)

	if count==0:
		separate = comment['time'].split(' ')
		separate = separate[0].split('-')
		year=separate[0]
		month=separate[1]
		day=separate[2]
		time=(int(year)-2000)*365+int(month)*30+int(day)

	if float(comment['sentiment_score'])<0:
		count+=1

	if cur_time-time>6:
		tempTuple={'Sentiment':'Negative',
				   'Time:':str(year)+'-'+str(month)+'-'+str(day)+' to '+str(year_cur)+'-'+str(month_cur)+'-'+str(day_cur),
				   'Count':count,
				   'Total Size Over Time':count_cur,
				   'Ratio':count/count_cur
		}
		his.append(tempTuple)
		count=0
		count_cur=0



with open('sentHisLarge.json','w') as outfile:
	json.dump(his,outfile,indent=2)

print('count total: '+str(count_total))

his=[]
count=0

for comment in output:
	count_cur+=1
	separate = comment['time'].split(' ')
	separate = separate[0].split('-')
	year_cur=separate[0]
	month_cur=separate[1]
	day_cur=separate[2]
	cur_time=(int(year_cur)-2000)*365+int(month_cur)*30+int(day_cur)


	if count==0:
		separate = comment['time'].split(' ')
		separate = separate[0].split('-')
		year=separate[0]
		month=separate[1]
		day=separate[2]
		time=(int(year)-2000)*365+int(month)*30+int(day)

	if comment['emotion_joy']>(emot_joy_avg+emot_joy_std):
		count+=1

	if cur_time-time>6 and count!=0:
		tempTuple={'Emotion':'joy',
				   'Time:':str(year)+'-'+str(month)+'-'+str(day)+' to '+str(year_cur)+'-'+str(month_cur)+'-'+str(day_cur),
				   'Count':count,
				   'Total Size Over Time':count_cur,
				   'Ratio':count/count_cur
		}

		print(tempTuple)
		his.append(tempTuple)
		count=0
		count_cur=0

with open('emotJoyHisLarge.json','w') as outfile:
	json.dump(his,outfile,indent=2)

	#print(hour)
	#print(separate[])



#data_topic=pd.read_json('data.json',encoding='utf-8',sep=',')
#print(json.dumps(response, indent=2))

#with open('data.json','w') as outfile:
# json.dump(response,outfile,indent=2)