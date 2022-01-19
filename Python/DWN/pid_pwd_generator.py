# This program randomly generates a list of 9-digit alphanumeric patient IDs and associated 9-digit numeric pwd
import random
import time
import string
import json
import datetime
import secrets

random.seed(time.time())
alphabet = string.ascii_letters + string.digits

# Designate number of patients
num_patients = 0
while num_patients <= 0:
    num_patients = input("How many patients would you like to generate?\n")
    try:
        num_patients = int(num_patients)
    except:
        num_patients = -1

# Generate patient data
entries = {}
entries['patients'] = {}
old_pid = ''
while len(entries['patients']) < num_patients:
    while True:
        new_pid = ''.join(secrets.choice(alphabet) for _ in range(9))
        if (any(c.islower() for c in new_pid)
                and any(c.isupper() for c in new_pid)
                and sum(c.isdigit() for c in new_pid) >= 1):
            break

    new_pwd = ''.join(secrets.choice(string.digits) for i in range(9))

    if new_pid not in entries['patients']:
        entries['patients'][new_pid] = new_pwd

fileName = ''
while fileName == '':
    try:
        fileName = str(input('What would you like to name the file? (no extension name)\n')) + '.json'
    except:
        fileName = ''

with open(fileName, "w") as outfile:
    json.dump(entries, outfile)
