import requests
import json
import datetime

# x = '{ "def":"isvalid", "id":"91283a1ac"}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
# print(r.json())

# param = '{ "def":"signup", "id":"a6360690a", "pin":724987}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=param)
# print(r.json()) # result 1 means that it worked

param = '{ "def":"login", "id":"a686ee6c1", "pin":123456789}'
r = requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)
print(r.json())  # result 1 means that it worked

# add patient into file
# y = '[{"qid":"2", "qtype":"Single", "answers": "Yes"}]' # this is an embedded json
# x = '{ "def":"insert", "id":"alex02468", "pin":724987, "date":"2020-08-30 13:13:09 +0000", "questions":[' \
#    '{"qid":"2", "qtype":"Single", "answers": "Yes"}' \
#    ']}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
# print(r.json()) # result 1 means that it worked

# for i in range(0,100):
#
#     ct = datetime.datetime.now()
#     ct = str(ct)
#     YMD = ct.split(" ")[0]
#     time = ct.split(" ")[1]
#     hour = time.split(":")[0]
#     min = time.split(":")[1]
#     sec = time.split(":")[2]
#     f_sec = sec.split(".")[0] + " +0000"
#
#     f_time = YMD + " " + hour + ":" + min + ":" + f_sec
#     #print(f_time)
#
#     a = {"qid":"3", "qtype":"Multiple", "answers": "Fever or chills", "aid": "a"}
#     b = {"qid":"3", "qtype":"Multiple", "answers": "cough", "aid": "b"}
#
#     c = json.dumps([a,b])
#     x = '{"def":"insert", "id":"a6360690a", "pin":724987, "date":\"'+f_time+'\", "questions": '+c+'}'
#     #print(x)
#
#     r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
#     print(r.json()) # result 1 means that it worked
#     print(i)
#end of for loop

