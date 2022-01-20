# This program randomly generates a list of 9-digit alphanumeric patient IDs and associated 9-digit numeric pwd
import random
import time
import string
import json
import csv
import secrets

# designate csv header
header = ['PID', 'PIN', 'First Name', 'Last Name', 'Email']

# seed randsom number generation
random.seed(time.time())

# define what characters are able to be used in PID
alphabet = string.ascii_letters + string.digits

# Designate number of patients (must be greater than 0)
num_patients = 0
while num_patients <= 0:
    num_patients = input("How many patients would you like to generate?\n")
    try:
        num_patients = int(num_patients)
    except:
        num_patients = -1

# get file name from user
fileName = ''
while fileName == '':
    try:
        fileName = str(input('What would you like to name the file? (no extension name)\n')) + '.csv'
    except:
        fileName = ''

# write to file
with open(fileName, "w", encoding='UTF8', newline='') as outfile:
    # create writer
    writer = csv.writer(outfile)

    # add header
    writer.writerow(header)

    # Generate patient data
    entries = dict()
    entries['patients'] = {}
    while len(entries['patients']) < num_patients:
        while True:
            # generate pid consisting of alphanumerics both upper and lowercase
            new_pid = ''.join(secrets.choice(alphabet) for _ in range(9))
            if (any(c.islower() for c in new_pid)
                    and any(c.isupper() for c in new_pid)
                    and sum(c.isdigit() for c in new_pid) >= 1):
                break

        # generate new password (not required to be unique between patients)
        new_pwd = ''.join(secrets.choice(string.digits) for i in range(9))

        # check if entry already exists in list of entries
        if new_pid not in entries['patients']:
            # add json entry
            entries['patients'][new_pid] = new_pwd
            # add csv data row
            writer.writerow([new_pid, new_pwd, '', '', ''])


# optional json generation
# fileName = fileName.replace('.csv', '.json')
# with open(fileName, "w") as outfile:
#     json.dump(entries, outfile)
