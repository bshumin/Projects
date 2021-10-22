import numpy as np
import matplotlib.pyplot as plt
import math
import random as rand

plt.rcParams['font.size'] = '30'
plt.rcParams['font.family'] = 'serif'

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
        gTruth_array = []
        meas_array = []

        for line in fpt.readlines():
            temp_array = line.replace('\n', '').split(' ')
            for i in range(len(temp_array)):
                temp_array[i] = float(temp_array[i])
            if temp_array:  # use implicit boolean property of list
                gTruth_array.append(temp_array[0])
                meas_array.append(temp_array[1])
        fpt.close()

        return [gTruth_array, meas_array]
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
        fxtm1tm1 = np.mat([[Xt[0, 0] + Xt[1, 0] * T], [Xt[1, 0]], [math.sin(Xt[0, 0] / 10)]])
        # predict next state
        Xt = fxtm1tm1
        gxtnt = [Xt[2, 0]]
        dfdx = np.mat([[1, T, 0],
                       [0, 1, 0],
                       [(1 / 10) * math.cos(Xt[0, 0] / 10), 0, 0]])
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


# get data
[true_data_1, meas_data_1] = readData("data1.txt")

# show data and measurements
plt.plot(np.linspace(0, 1 * len(true_data_1), len(true_data_1)), true_data_1, 'black')
plt.scatter(x=np.linspace(0, 1 * len(true_data_1), len(true_data_1)), y=meas_data_1, marker='.', c="grey")
plt.xlabel('Time')
plt.ylabel('Position')
plt.show()
plt.close()

# EKF calls
extended_kalman_sin10(Q=.01, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
extended_kalman_sin10(Q=.001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
extended_kalman_sin10(Q=.0001, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')
extended_kalman_sin10(Q=.004, R=1, T=1, pos=meas_data_1, true_pos=true_data_1, title='')  # idealish EKF
