import requests
import json
import datetime
from threading import Thread
from time import process_time
import time
import warnings
from queue import Queue
from statistics import stdev
from matplotlib import pyplot as plt
import matplotlib
from time import sleep
import numpy
warnings.filterwarnings("ignore")

# Queueing Notes: https://www.shanelynn.ie/using-python-threading-for-multiple-results-queue/
# TODO: SEE BELOW
# 6:00am-8:00pm = 95% of data
# find statistics of mobile phone usage
# simulate 1 hour of data at stress thresholds
# simulate 24 minutes of data as a full day of testing


# This function is used to send an http request to login a patient
def request_login():
    param = 'YOUR_PARAMS_HERE'
    return requests.post('YOUR_ADDRESS_HERE', data=param, verify=False)


# This function runs a set of threads to call the request_login() function
# -params: threads - number of threads (simulated patients) to run
# NOTE: this function attempts to run all threads simultaneously, so at high numbers of threads, this acts as a stress
#       test of the server
def thread_login(threads):
    # create array of threads to be run
    threads = [Thread(target=request_login) for _ in range(threads)]

    # record start time of requests
    start_time = process_time()

    # start all threads
    for t in threads:
        t.start()

    # wait for all threads to complete
    for t in threads:
        t.join()

    # record stop time
    stop_time = process_time()
    thread_time = stop_time - start_time
    data = []
    data.append({'time': thread_time,
                 'success': True})
    return parse_data(data, thread_time, len(threads))


# This function acts as a helper function for queued requests. It is the process that is threaded in queue_login(), and
# runs the provided queue until all processes have been completed. Request statistics are also recorded here such as
# time to complete the request and if the process succeeded or not.
# -params: q - the Queue containing all the threads to be run
#          data - the array containing all the data from the threads being run
# -return: True when function exits
# NOTE: the data is stored as follows - [{'time': time for the request to return a result
#                                         'success': success of the request (True means completed, False means error)}]
def crawl(q,  data):
    while not q.empty():
        work = q.get()  # fetch new work from the Queue
        success = False

        # get start time for request
        start_time = process_time()

        # attempt the request
        try:
            work.start()
            success = True
        except:
            print('Error with request!')

        # calculate total time for request and append data
        thread_time = process_time() - start_time
        data.append({'time': thread_time,
                     'success': success})

        # signal to the queue that task has been processed
        q.task_done()
    return True


# This function creates and threads the queued requests.
# -params: requests_total - number of requests total to simulate
#          batch_size - number of threads max to run in parallel
# -return: array of the total time it took all threads to complete and the runtime data from the threads
def queue_login(requests_total, batch_size=50):
    # create queue
    q = Queue(maxsize=batch_size)

    # determine how many threads to run in parallel and create data
    thread_batch = min(requests_total, batch_size)
    data = []

    # put all requests to complete on queue
    for _ in range(requests_total):
        q.put(Thread(target=request_login))

    # record start time of all processes
    start_time = process_time()

    # start number of threads equal to the amount of threads desired at a a time
    for _ in range(thread_batch):
        worker = Thread(target=crawl, args=(q, data))  # create worker thread
        worker.setDaemon(True)  # setting as daemon allows for function to still complete even if a thread gets stuck
        worker.start()  # start worker thread

    # wait for all threads to complete
    q.join()

    # record the time at which all threads complete and calculate total thread time
    stop_time = process_time()
    total_time = stop_time - start_time
    return parse_data(data, total_time, batch_size)


# This function processes thread data and outputs calculated values from this data.
# -params: data - the thread data including success and runtime of requests
#          total_time - total time it took for all requests to complete in the given batch of requests
#          concurrent_requests - number of threads running in parallel for the set of data (batch_size for queue)
# -return: parsed_data - the formatted data in a set of  key-value pairs
def parse_data(data, total_time, concurrent_requests):

    # initialize sum, failure, longest request request_time, entry values, and array of time values for stdev
    sum_time = 0
    failure = 0
    lrt = 0
    num_entries = len(data)
    if num_entries == 1:
        num_entries = concurrent_requests  # stress test case
    vals = []

    # get through put by formula total_requests/total_time_for_requests
    throughput = num_entries/total_time

    # iterate through each request's recorded values
    for entry in data:
        # add time values to sum
        sum_time += entry['time']

        # record number of errors
        if not entry['success']:
            failure += 1

        # check for longest request time (lrt)
        if entry['time'] > lrt:
            lrt = entry['time']
        # keep list of all times for standard deviation
        vals.append(entry['time'])
    avg_load_time = sum_time/num_entries

    # ==================================================================================================================
    # requests - number of requests processed
    # concurrent_users - number of threads run in paralel (batch_size for queue)
    # error_rate - ratio of failed to successful requests
    # throughput - rate of requests per second for this simulation
    # avg_load_time - the calculated mean time it took for a request to be sent, processed on the server, and returned
    # longest_request_time - the longest time it took for a single request to complete
    # total_time - the total time taken for all threads to complete (not total thread request_time, since they are in parallel)
    # std_dev - the standard deviation for the request times
    # ==================================================================================================================
    try:
        parsed_data = {'requests': num_entries,
                       'concurrent_users': concurrent_requests,
                       'error_rate': failure/num_entries,
                       'throughput': throughput,
                       'avg_load_time': avg_load_time,
                       'longest_request_time': lrt,
                       'total_time': total_time,
                       'std_dev': stdev(vals)}
    except:
        parsed_data = {'requests': num_entries,
                       'concurrent_users': concurrent_requests,
                       'error_rate': failure / num_entries,
                       'throughput': throughput,
                       'avg_load_time': avg_load_time,
                       'longest_request_time': lrt,
                       'total_time': total_time,
                       'std_dev': 0}
    return parsed_data


# This function averages data from the thread ir queue functions and
# -params: data - the data from a single run of a batch of data
#          batch_size - the amount of processes batched together at once
# -return: parsed_data - the formatted data in a set of  key-value pairs
def avg_data(data, batch_size):

    r = 0
    cu = 0
    er = 0
    tp = 0
    alt = 0
    lrt = 0
    tt = 0
    sd = 0
    num = 0
    for num, item in enumerate(data):
        r += item['requests']
        cu += item['concurrent_users']
        er += item['error_rate']
        tp += item['throughput']
        alt += item['avg_load_time']
        lrt += item['longest_request_time']
        tt += item['total_time']
        sd += item['std_dev']

    num = num+1
    # average data
    r /= num
    cu /= num
    er /= num
    tp /= num
    alt /= num
    lrt /= num
    tt /= num
    sd /= num

    return {'requests': r,
            'concurrent_users': cu,
            'error_rate': er,
            'throughput': tp,
            'avg_load_time': alt,
            'longest_request_time': lrt,
            'total_time': tt,
            'std_dev': sd,
            'batch_size': batch_size}


def run_batch(range_arg, np, bs):
    mini_batch = []
    for _ in range(range_arg):
        mini_batch.append(queue_login(np, batch_size=bs))
    return mini_batch


# x = '{ "def":"isvalid", "id":"91283a1ac"}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
# print(r.json())

# param = '{ "def":"signup", "id":"a6360690a", "pin":724987}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=param)
# print(r.json()) # result 1 means that it worked

# r = requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)
# print(r.json())  # result 1 means that it worked

# Stress Test ==========================================================================================================
queue_data = []
batch_data = []
range_arg = 5
np = 0
bs_min = 1000
bs_max = 5500
increment = 500
i = 1
for np in range(bs_min, bs_max, increment):
    queue_data.append(avg_data(run_batch(range_arg, np, np), np))
    batch_data.append(np)
    print(*['Batch', i, 'complete'])
    i += 1
    sleep(10)

avgLoadTime = []
throughput = []
longestRequestTime = []
totalTime = []
error_rate = []
for i in queue_data:
    avgLoadTime.append(i['avg_load_time'])
    throughput.append(i['throughput'])
    longestRequestTime.append(i['longest_request_time'])
    totalTime.append(i['total_time'])
    error_rate.append(i['error_rate'])

plt.plot(batch_data, avgLoadTime)
plt.title('Average Response Time')
plt.xticks(numpy.arange(bs_min, bs_max, increment))
plt.xlabel('Number of users per batch')
plt.ylabel('Time (seconds)')
plt.grid()
plt.show()

plt.plot(batch_data, throughput)
plt.title('Throughput')
plt.xticks(numpy.arange(bs_min, bs_max, increment))
plt.xlabel('Number of users per batch')
plt.ylabel('Responses per second')
plt.grid()
plt.show()

plt.plot(batch_data, longestRequestTime)
plt.title('Longest Request Time')
plt.xticks(numpy.arange(bs_min, bs_max, increment))
plt.xlabel('Number of users per batch')
plt.ylabel('Time (seconds)')
plt.grid()
plt.show()

plt.plot(batch_data, totalTime)
plt.title('Total Request Time')
plt.xticks(numpy.arange(bs_min, bs_max, increment))
plt.xlabel('Number of users per batch')
plt.ylabel('Time (seconds)')
plt.grid()
plt.show()

plt.plot(batch_data, error_rate)
plt.title('Error Rate')
plt.xticks(numpy.arange(bs_min, bs_max, increment))
plt.yticks(numpy.arange(0, .25, .05))
plt.xlabel('Number of users per batch')
plt.ylabel('Error Rate')
plt.grid()
plt.show()

# ======================================================================================================================

# Load Test 1 ==========================================================================================================
# queue_data = []
# batch_data = []
# range_arg = 5
# np = 5000
# bs_min = 100
# bs_max = 600
# for bs in range(bs_min, bs_max, 100):
#     queue_data.append(avg_data(run_batch(range_arg, np, bs), bs))
#     batch_data.append(bs)
#     print(*['Batch', bs, 'complete'])
#     sleep(60)
#
# avgLoadTime = []
# throughput = []
# longestRequestTime = []
# totalTime = []
# for i in queue_data:
#     avgLoadTime.append(i['avg_load_time'])
#     throughput.append(i['throughput'])
#     longestRequestTime.append(i['longest_request_time'])
#     totalTime.append(i['total_time'])
#
# plt.plot(batch_data, avgLoadTime)
# plt.title('Average Response Time')
# plt.xticks(numpy.arange(100, bs_max, 100))
# plt.xlabel('Number of users per batch')
# plt.ylabel('Time (seconds)')
# plt.grid()
# plt.show()
#
# plt.plot(batch_data, throughput)
# plt.title('Throughput')
# plt.xticks(numpy.arange(100, bs_max, 100))
# plt.xlabel('Number of users per batch')
# plt.ylabel('Responses per second')
# plt.grid()
# plt.show()
#
# plt.plot(batch_data, longestRequestTime)
# plt.title('Longest Request Time')
# plt.xticks(numpy.arange(100, bs_max, 100))
# plt.xlabel('Number of users per batch')
# plt.ylabel('Time (seconds)')
# plt.grid()
# plt.show()
#
# plt.plot(batch_data, totalTime)
# plt.title('Total Request Time')
# plt.xticks(numpy.arange(100, bs_max, 100))
# plt.xlabel('Number of users per batch')
# plt.ylabel('Time (seconds)')
# plt.grid()
# plt.show()

# ======================================================================================================================

# constant performance test? ===========================================================================================
# stress_x = []
# stress_alt = []
# stress_tt = []
# batch_min = 2000
# batch_max = 20000
# for batch in range(batch_min, batch_max+batch_min, batch_min):
#     thread_data = thread_login(batch)
#     stress_x.append(thread_data['requests'])
#     stress_alt.append(thread_data['avg_load_time'])
#     stress_tt.append(thread_data['total_time'])
#     print(thread_data)
#     sleep(20)
#
# plt.plot(stress_x, stress_alt)
# plt.title('Average Response Time')
# plt.xticks(numpy.arange(batch_min, batch_max, 2000))
# plt.xlabel('Number of users')
# plt.ylabel('Time (seconds)')
# plt.grid()
# plt.show()
#
# plt.plot(stress_x, stress_tt)
# plt.title('Total Response Time')
# plt.xticks(numpy.arange(batch_min, batch_max, 2000))
# plt.xlabel('Number of users')
# plt.ylabel('Time (seconds)')
# plt.grid()
# plt.show()

# ======================================================================================================================

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
