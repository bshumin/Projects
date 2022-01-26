import requests
import json
import datetime
from threading import Thread
from time import clock, clock
import warnings
from queue import Queue
from statistics import stdev
warnings.filterwarnings("ignore")

# TODO: Investigate queueing https://www.shanelynn.ie/using-python-threading-for-multiple-results-queue/


# This function is used to send an http request to login a patient
def request_login():
    param = '{ "def":"login", "id":"a686ee6c1", "pin":123456789}'
    return requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)


# This function runs a set of threads to call the request_login() function
# -params: threads - number of threads (simulated patients) to run
# NOTE: this function attempts to run all threads simultaneously, so at high numbers of threads, this acts as a stress
#       test of the server
def thread_login(threads):
    # create array of threads to be run
    threads = [Thread(target=request_login) for _ in range(threads)]

    # record start time of requests
    start_time = clock()

    # start all threads
    for t in threads:
        t.start()

    # wait for all threads to complete
    for t in threads:
        t.join()

    # record stop time
    stop_time = clock()
    
    return stop_time - start_time


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
        start_time = clock()

        # attempt the request
        try:
            work.start()
            success = True
        except:
            print('Error with request!')

        # calculate total time for request and append data
        thread_time = clock() - start_time
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
    q = Queue(maxsize=0)

    # determine how many threads to run in parallel and create data
    thread_batch = min(requests_total, batch_size)
    data = []

    # put all requests to complete on queue
    for _ in range(requests_total):
        q.put(Thread(target=request_login))

    # record start time of all processes
    start_time = clock()

    # start number of threads equal to the amount of threads desired at a a time
    for _ in range(thread_batch):
        worker = Thread(target=crawl, args=(q, data))  # create worker thread
        worker.setDaemon(True)  # setting as daemon allows for function to still complete even if a thread gets stuck
        worker.start()  # start worker thread

    # wait for all threads to complete
    q.join()

    # record the time at which all threads complete and calculate total thread time
    stop_time = clock()
    total_time = stop_time - start_time

    return [total_time, data]


# This function processes thread data and outputs calculated values from this data.
# -params: data - the thread data including success and runtime of requests
#          total_time - total time it took for all requests to complete in the given batch of requests
#          concurrent_requests - number of threads running in parallel for the set of data (batch_size for queue)
# -return: parsed_data - the formatted data in a set of  key-value pairs
def parse_data(data, total_time, concurrent_requests):

    # initialize sum, failure, longest request time, entry values, and array of time values for standard deviation
    sum_time = 0
    failure = 0
    lrt = 0
    num_entries = len(data)
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
    # total_time - the total time taken for all threads to complete (not total thread time, since they are in parallel)
    # std_dev - the standard deviation for the request times
    # ==================================================================================================================
    parsed_data = {'requests': num_entries,
                   'concurrent_users': concurrent_requests,
                   'error_rate': failure/num_entries,
                   'throughput': throughput,
                   'avg_load_time': avg_load_time,
                   'longest_request_time': lrt,
                   'total_time': total_time,
                   'std_dev': stdev(vals)}
    return parsed_data


# x = '{ "def":"isvalid", "id":"91283a1ac"}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=x)
# print(r.json())

# param = '{ "def":"signup", "id":"a6360690a", "pin":724987}'
# r = requests.post('http://mahi-labs.com/CovidSxTracker.php', data=param)
# print(r.json()) # result 1 means that it worked

# r = requests.post('https://app.digitalwellnessnurse.com/dwn.php', data=param, verify=False)
# print(r.json())  # result 1 means that it worked

# time = thread_login(10)
# print(time)
# time = thread_login(100)
# print(time)
# time = thread_login(1000)
# print(time)

[time, data] = queue_login(50)
print(parse_data(data, time, 50))
# print("%.2f" % time)
# print(data)
[time, data] = queue_login(100)
print(parse_data(data, time, 50))
# print("%.2f" % time)
# print(data)
[time, data] = queue_login(800)
print(parse_data(data, time, 50))
# print("%.2f" % time)
# print(data)
[time, data] = queue_login(1600)
print(parse_data(data, time, 50))
# print("%.2f" % time)
# print(data)


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
