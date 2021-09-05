import numpy as np
import matplotlib.pyplot as plt
import sympy as sp
import math
# NOTE f(x) = ln(ax), f'(x) = (1/x)


def ln(x_value):
    return math.log(x_value)


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


def next_guess(an, x, y):
    return an - (f(an, x, y)/f_prime(an, x, y))


def f_prime(an, x, y):
    fpan = 0.0
    for i in range(len(x)):
        fpan += 2*(-ln(an*x[i])+y[i]+1)/pow(an, 2)
    return fpan


def f(an, x, y):
    fan = 0.0
    for i in range(len(x)):
        try:
            fan += -2*(y[i]-ln(an*x[i]))/an
        except:
            fan += 0
    return fan


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


last_guess = float(input('Select a value as the initial guess: '))
iterations = 0
while True:
    new_guess = next_guess(last_guess, x_a, y_a)
    iterations += 1
    print(str(last_guess) + ' -> ' + str(new_guess))
    if abs(last_guess-new_guess) < 0.001:
        a = new_guess
        print('Number of iterations: ' + str(iterations) + '\n')
        break
    else:
        last_guess = new_guess

xa_2 = np.linspace(0.5, max(x_a), 10000)
ya_2 = []
for val in xa_2:
    ya_2.append(ln(a*val))

plt.plot(xa_2, ya_2)
plt.show()
plt.close()


# import data B --------------------------------------------------------------------------------------------------------
data_b = readData("log-data-B.txt")
x_b = []
y_b = []
for val in data_b:
    x_b.append(val[0])
    y_b.append(val[1])

# show data as it appears
plt.scatter(x_b, y_b, marker='o')
plt.grid(which='major')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Data B')


last_guess = float(input('Select a value as the initial guess: '))
iterations = 0
while True:
    new_guess = next_guess(last_guess, x_b, y_b)
    iterations += 1
    print(str(last_guess) + ' -> ' + str(new_guess))
    if abs(last_guess-new_guess) < 0.001:
        a = new_guess
        print('Number of iterations: ' + str(iterations) + '\n')
        break
    else:
        last_guess = new_guess

xb_2 = np.linspace(0.5, max(x_b), 10000)
yb_2 = []
for val in xb_2:
    yb_2.append(ln(a*val))

plt.plot(xb_2, yb_2)
plt.show()
plt.close()

# import data C --------------------------------------------------------------------------------------------------------
data_c = readData("log-data-C.txt")
x_c = []
y_c = []
for val in data_c:
    x_c.append(val[0])
    y_c.append(val[1])

# show data as it appears
plt.scatter(x_c, y_c, marker='o')
plt.grid(which='major')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Data C')

last_guess = float(input('Select a value as the initial guess: '))
iterations = 0
while True:
    new_guess = next_guess(last_guess, x_c, y_c)
    iterations += 1
    print(str(last_guess) + ' -> ' + str(new_guess))
    if abs(last_guess-new_guess) < 0.001:
        a = new_guess
        print('Number of iterations: ' + str(iterations) + '\n')
        break
    else:
        last_guess = new_guess

xc_2 = np.linspace(0.5, max(x_c), 10000)
yc_2 = []
for val in xc_2:
    yc_2.append(ln(a*val))

plt.plot(xc_2, yc_2)
plt.show()
plt.close()
