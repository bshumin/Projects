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


# create an extended kalman filter of model type sin/10
def extended_kalman_sin10(Q, R, T, pos, true_pos, title):

    dfda = np.mat([[0, 0, 0],
                   [0, 1, 0],
                   [0, 0, 0]])
    dgdx = np.mat([[0, 0, 1]])
    dgdn = np.mat([1])

    St = np.mat([[1, 0, 0],
                 [0, 1, 0],
                 [0, 0, 1]])
    Xt = np.mat([0, 0, 0]).getT()
    pred = []

    Q = np.mat([[0, 0, 0],
                [0, Q, 0],
                [0, 0, 0]])
    R = np.mat([R])

    for val in range(len(pos)):
        fxtm1tm1 = np.mat([[Xt[0, 0] + Xt[1, 0] * T], [Xt[1, 0]], [sin(Xt[0, 0] / 10)]])
        # predict next state
        Xt = fxtm1tm1
        gxtnt = [Xt[2, 0]]
        dfdx = np.mat([[1, T, 0],
                       [0, 1, 0],
                       [(1 / 10) * cos(Xt[0, 0] / 10), 0, 0]])
        # predict next state covariance
        St = dfdx*St*dfdx.getT() + dfda*Q*dfda.getT()
        # Obtain measurement
        Yt = pos[val]
        # calculate Kalman gain
        Kt = St*dgdx.getT()*pow((dgdx*St*dgdx.getT() + dgdn*R*dgdn.getT()), -1)
        # update state
        Xt = Xt + Kt*(Yt - gxtnt[0])
        # update state covariance
        St = (np.identity(3)-Kt*dgdx)*St
        pred.append(Xt[2][0])

    pred = np.squeeze(np.asarray(pred))
    print(pred)

    plt.plot(np.linspace(0, T * len(pos), len(pos)), pred, 'black', linestyle="-")
    plt.plot(np.linspace(0, T * len(pos), len(pos)), true_pos, 'grey', linestyle=":")
    # plt.scatter(x=np.linspace(0, T * len(pos), len(pos)), y=pos, marker='.')
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.title(title)
    plt.show()
    plt.close()


def particle_filter(pos, vel, meas, T, Q, R):
    Xt = np.mat([[0], [0]])
    Xm = []
    sigm = 4.0
    sign = pow(2, (-1)*8)
    xm1 = -10
    xm2 = 10
    nt = 0  # change to random sample from N(0, sign^2)
    for val in range(len(pos)):
        fx = np.mat([[Xt[0, 0]+Xt[1, 0]*T], [np.piecewise(Xt[0, 0], [Xt[0, 0] < -20, -20 <= Xt[0, 0] < 0,
                    0 <= Xt[0, 0] <= 20, Xt[0, 0] > 20], [2, Xt[1, 0] + abs(Q), Xt[1, 0] - abs(Q), -2])]])
        Yt = np.mat(pos[val])
        gx = np.mat(1 / (sqrt(2 * pi) * sigm) * exp((-pow(Xt[0, 0] - xm1, 2)) / (2 * pow(sigm, 2))) +
                    1 / (sqrt(2 * pi) * sigm) * exp((-pow(Xt[0, 0] - xm2, 2)) / (2 * pow(sigm, 2))) + nt)


# get data
[act_pos, act_vel, meas_pos] = readData("data.txt")

particle_filter(act_pos, act_vel, meas_pos, T=1, Q=0.002, R=0.002)

# EKF calls
# extended_kalman_sin10(Q=.01, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.0001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
# extended_kalman_sin10(Q=.004, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')  # idealish EKF
