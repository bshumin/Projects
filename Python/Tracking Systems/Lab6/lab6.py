import numpy as np
import matplotlib.pyplot as plt
from math import *
import random as rand

plt.rcParams['font.size'] = '30'
plt.rcParams['font.family'] = 'serif'


# helper functions
# read in data from a file
def readData(file_in):
    fpt = ''  # fixes error in IDE of fpt not existing in scope (try/except will never allow for it to not be in scope)
    try:
        fpt = open(file_in)
    except:
        print(file_in + ' could not be opened. Check the filename and try again.')
        exit()
    try:
        pos = []
        vel = []
        meas = []

        for line in fpt.readlines():
            temp_array = line.replace('\n', '').split('\t')
            for i in range(len(temp_array)):
                temp_array[i] = float(temp_array[i])
            if temp_array:  # use implicit boolean property of list
                pos.append(temp_array[0])
                vel.append(temp_array[1])
                meas.append(temp_array[2])
        fpt.close()

        return [pos, vel, meas]
    except:
        print('Something went wrong in parsing the data from ' + file_in)


def particle_filter(pos, vel, meas, T=1, M=300, ESS_Thresh=0.5):
    # instantiate Xm and Wm matrices
    Xm = []
    W = []
    for i in range(M):
        Xm.append([0, 0])
        W.append(1 / M)

    siga = 0.25
    sigm = 4.0
    sign = 0.003906
    xm1 = -10
    xm2 = 10

    # get prediction vals
    pos_pred = []
    vel_pred = []

    for val in range(len(pos)):

        Yt = meas[val]
        sum_W = 0
        # propagate particles and update weights and particle states
        for i in range(M):
            if Xm[i][0] < -20:
                fx = [Xm[i][0] + Xm[i][1] * T, 2]
            elif (-20 <= Xm[i][0]) and (Xm[i][0] < 0):
                fx = [Xm[i][0] + Xm[i][1] * T, Xm[i][1] + abs(rand.gauss(0, pow(siga, 2)))]
            elif (0 <= Xm[i][0]) and (Xm[i][0] <= 20):
                fx = [Xm[i][0] + Xm[i][1] * T, Xm[i][1] - abs(rand.gauss(0, pow(siga, 2)))]
            else:
                fx = [Xm[i][0] + Xm[i][1] * T, -2]

            gx = (1 / (sqrt(2 * pi) * sigm)) * exp((-pow(Xm[i][0] - xm1, 2)) / (2 * pow(sigm, 2))) + \
                 (1 / (sqrt(2 * pi) * sigm)) * exp((-pow(Xm[i][0] - xm2, 2)) / (2 * pow(sigm, 2)))
            py = (1 / (sqrt(2 * pi) * sign)) * exp((-pow(gx - Yt, 2)) / (2 * pow(sign, 2)))
            W[i] = W[i] * py
            sum_W += W[i]
            Xm[i][0] = fx[0]
            Xm[i][1] = fx[1]

        sum_pos = 0
        sum_vel = 0
        CV = 0
        for i in range(M):
            W[i] = W[i] / sum_W  # normalize weights
            sum_pos += W[i] * Xm[i][0]
            sum_vel += W[i] * Xm[i][1]
            CV += pow(M * W[i] - 1, 2)

        CV = (1 / M) * CV
        ESS = (M / (1 + CV))

        pos_pred.append(sum_pos)
        vel_pred.append(sum_vel)

        # resample here
        if ESS < (0.2*M):
            Q = np.cumsum(W)
            t = np.random.uniform(low=0.0, high=1.0, size=M)
            t_sort = np.sort(t)
            i = j = 0
            Index = [0] * M
            while i < M:
                if t_sort[i] < Q[j]:
                    Index[i] = j
                    i += 1
                else:
                    j += 1
            i = 0
            while i < M:
                Xm[i][0] = Xm[Index[i]][0]
                Xm[i][1] = Xm[Index[i]][1]
                W[i] = 1 / M
                i += 1

    pos_pred = np.squeeze(np.asarray(pos_pred))
    vel_pred = np.squeeze(np.asarray(vel_pred))

    plt.plot(np.linspace(0, T * len(pos), len(pos)), pos_pred, 'black', linestyle="-")
    plt.plot(np.linspace(0, T * len(pos), len(pos)), pos, 'grey', linestyle=":")
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.show()
    plt.close()

    plt.plot(np.linspace(0, T * len(pos), len(pos)), vel_pred, 'black', linestyle="-")
    plt.plot(np.linspace(0, T * len(pos), len(pos)), vel, 'grey', linestyle=":")
    plt.xlabel('Time')
    plt.ylabel('Velocity')
    plt.show()
    plt.close()


# get data
[act_pos, act_vel, meas_pos] = readData("data.txt")

particle_filter(act_pos, act_vel, meas_pos)

# EKF calls
# extended_kalman_sin10(Q=.01, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.0001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.004, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')  # idealish EKF
