import requests
import json
import datetime
from threading import Thread
from time import process_time
import warnings
from queue import Queue
import logging
warnings.filterwarnings("ignore")

# TODO: Investigate queueing https://www.shanelynn.ie/using-python-threading-for-multiple-results-queue/


def request_login():
    param = '{ "def":"login", "id":"a686ee6c1", "pin":123456789}'
    return requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)


def thread_load_test(threads):
    threads = [Thread(target=request_login) for _ in range(threads)]
    start_time = process_time()

    for t in threads:
        t.start()

    for t in threads:
        t.join()

    stop_time = process_time()
    return stop_time - start_time


def crawl(q, thread_num):
    while not q.empty():
        work = q.get()  # fetch new work from the Queue
        try:
            work.start()
            logging.info('Running thread {: }', thread_num)
        except:
            logging.error('Error with request!')
        # signal to the queue that task has been processed
        q.task_done()
    return True


def queue_test(threads):
    q = Queue(maxsize=0)
    num_threads = min(500, threads)
    start_time = process_time()

    for i in range(threads):
        q.put(Thread(target=request_login))

    for i in range(num_threads):
        worker = Thread(target=crawl, args=(q, i))
        worker.setDaemon(True)
        worker.start()

    q.join()

    stop_time = process_time()
    logging.info('All tasks completed.')

    return stop_time - start_time


# x = '{ "def":"isvalid", "id":"91283a1ac"}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
# print(r.json())

# param = '{ "def":"signup", "id":"a6360690a", "pin":724987}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=param)
# print(r.json()) # result 1 means that it worked

# r = requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)
# print(r.json())  # result 1 means that it worked

# time = thread_load_test(10)
# print(time)
# time = thread_load_test(100)
# print(time)
# time = thread_load_test(1000)
# print(time)

time = queue_test(50)
print(time)
time = queue_test(100)
print(time)
time = queue_test(800)
print(time)



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

