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
def readData(file_in, dim):

    fpt = ''  # fixes error in IDE of fpt not existing in scope (try/except will never allow for it to not be in scope)
    try:
        fpt = open(file_in)
    except:
        print(file_in + ' could not be opened. Check the filename and try again.')
        exit()
    try:
        file_array = []
        if dim == 1:
            for line in fpt.readlines():
                temp_array = line.replace('\n', '').split(' ')
                for i in range(len(temp_array)):
                    temp_array = float(temp_array[i])
                if temp_array:  # use implicit boolean property of list
                    file_array.append(temp_array)
        else:
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


def constantVelocity(dyn_noise, meas_noise, T, velocity, pos, title):
    vel_vec = velocity * np.ones(len(pos))
    Xt = [0]
    Xdott = [vel_vec[0]]
    GtHt = dyn_noise
    for val in range(len(pos)):
        # skip first value
        if val > 0:
            # state transition
            Yt = Xt[val-1] + rand.gauss(0, pow(meas_noise, 2))
            Xt.append(Xt[val-1]+GtHt*(Yt-Xt[val-1]))
            Xdott.append(Xdott[val-1] + GtHt*(Yt-Xt[val-1])/T)

            # update equations
            Xt[val-1] = Xt[val - 1] + Xdott[val - 1] * T
            Xdott[val-1] = Xdott[val-1] + rand.gauss(0, pow(meas_noise, 2))
    print(Xdott)
    print(Xt)
    M = [[1, 0, 0, 0], [0, 1, 0, 0]]
    PHI = [[1, T], [0, 1]]

    plt.plot(np.linspace(0, T * len(pos), len(pos)), Xt)
    plt.plot(np.linspace(0, T * len(pos), len(pos)), pos)
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.title(title)
    plt.show()


Pos = readData("1D-data.txt", 1)
data_2D = readData("2D-data.txt", 2)

# constant velocity model

constantVelocity(dyn_noise=.2, meas_noise=.2, T=1, velocity=0, pos=Pos, title='1D-Velocity - Balanced Q and R')
constantVelocity(dyn_noise=0, meas_noise=1, T=1, velocity=0, pos=Pos, title='1D-Velocity - All Q')
constantVelocity(dyn_noise=1, meas_noise=0.01, T=1, velocity=0, pos=Pos, title='1D-Velocity - All R')