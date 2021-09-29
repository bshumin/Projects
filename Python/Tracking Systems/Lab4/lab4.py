import numpy as np
import matplotlib.pyplot as plt
import sympy as sp
import math
import random as rand

# Applying the KF
# (1) determine state variables
# (2) write state transition equations (nominal behavior)
# (3) define dynamic noise (unexpected behavior)
# (4) determine observations (sensors)
# (5) write observation equations (relate (4) to (1))
# (6) define measurement noise
# (7) build covariance matrices
# (8) build state transition and observation matrices
# (9) check all matrix sizes

# 2D constant velocity model
# (1) xt = [xt;yt;x't;y't]
# (2)
#   xt+1  = xt + Tx't
#   yt+1  = yt + Ty't
#   x't+1 = x't
#   y't+1 = y't
# (3) dynamic noise = [0;0;N(0, sigma(n1)^2);N(0, sigma(n2)^2)]
# (4) Yt = [x~t;y~t]
# (5)
#   x~t = xt
#   y~t = yt
# (6) measurement noise = [sigma(n1)^2);N(0, sigma(n2)^2)]
# (7) cov(state) = cov(x) =
#  NOTE: include cov(Q), cov(R) and PHI matrices in report as well


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
                temp_array = float(temp_array[i])
            if temp_array:  # use implicit boolean property of list
                file_array.append(temp_array)

        fpt.close()

        return file_array
    except:
        print('Something went wrong in parsing the data from ' + file_in)


def constantVelocity():
    print()



pos = readData("1D-data.txt")


# Control variables ================================================================================================== #
dyn_noise = 1
meas_noise = 0.3
T = 1  # Timing variable
velocity = 2
# ==================================================================================================================== #

# constant velocity model
vel_vec = velocity * np.ones(len(pos))
x_vals = [pos, vel_vec]
Xmeas = np.matrix(x_vals)  # measured values
print(Xmeas)

Xt = [pos[0]]
Xdott = [vel_vec[0]]
for val in range(len(pos)):
    # skip initialization
    if val > 0:
        # state transition
        Xt.append(pos[val-1] + Xdott[val-1]*T)
        # dynamic noise
        Xdott.append(Xdott[val-1] + rand.gauss(0, pow(dyn_noise, 2)))

print(Xt)
M = [[1, 0, 0, 0], [0, 1, 0, 0]]
PHI = [[1, T], [0, 1]]

print(len(Xt))
print(len(pos))

plt.plot(np.linspace(0, T*len(pos), len(pos)), Xt)
plt.plot(np.linspace(0, T*len(pos), len(pos)), pos)
plt.xlabel('Time')
plt.ylabel('Position')
plt.title('1D-Velocity - 1')
plt.show()


