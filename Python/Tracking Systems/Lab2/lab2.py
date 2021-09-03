import numpy as np
import matplotlib.pyplot as plt
import sympy as sp
# NOTE f(x) = ln(ax), f'(x) = (1/x)


# helper functions
def readData(file_in):

    fpt = ''  # fixes error in IDE of fpt not existing in scope (try/except will never allow for it to not be in scope)
    try:
        fpt = open(file_in)
    except:
        print(file_in + ' could not be opened. Check the filename and try again.')
        exit()
    try:
        file_array = []

        for line in fpt.readlines():
            temp_array = line.replace('\n', '').split(' ')
            for i in range(len(temp_array)):
                temp_array[i] = float(temp_array[i])
            if temp_array:  # use implicit boolean property of list
                file_array.append(temp_array)

        fpt.close()

        return file_array
    except:
        print('Something went wrong in parsing the data from ' + file_in)


# import data A --------------------------------------------------------------------------------------------------------
data_a = readData("log-data-A.txt")
x_a = []
y_a = []
for val in data_a:
    x_a.append(val[0])
    y_a.append(val[1])

# show data as it appears
plt.scatter(x_a, y_a, marker='o')
plt.grid(which='major')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Data A')
plt.show()
plt.close()

# import data B --------------------------------------------------------------------------------------------------------
data_b = readData("log-data-B.txt")
x_b = []
y_b = []
for val in data_a:
    x_b.append(val[0])
    y_b.append(val[1])

# show data as it appears
plt.scatter(x_b, y_b, marker='o')
plt.grid(which='major')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Data B')
plt.show()
plt.close()

# import data C --------------------------------------------------------------------------------------------------------
data_c = readData("log-data-C.txt")
x_c = []
y_c = []
for val in data_a:
    x_c.append(val[0])
    y_c.append(val[1])

# show data as it appears
plt.scatter(x_c, y_c, marker='o')
plt.grid(which='major')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Data C')
plt.show()
plt.close()
