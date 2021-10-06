import numpy as np
import matplotlib.pyplot as plt
import sympy as sp
import math
import random as rand

plt.rcParams['font.size'] = '20'

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


def kalman1D(Q, R, T, pos, title):
    M = np.mat([1, 0])

    PHI = np.mat([[1, T], [0, 1]])
    Xt = np.mat([0, 0]).getT()  # x_{t-1,t-1}
    St = np.mat([[1, 0], [0, 1]])
    Y_pred = []
    Q = np.mat([[0, 0], [0, Q]])
    R = np.mat(R)

    for val in range(len(pos)):
        # predict next state
        Xttm1 = PHI*Xt
        # predict next state covariance
        Sttm1 = PHI*St*PHI.getT() + Q
        # Obtain measurement
        Yt = pos[val]
        # calculate Kalman gain
        Kt = Sttm1*M.getT()*np.mat(M*Sttm1*M.getT() + R).getI()
        # update state
        Xt = Xttm1 + Kt*(Yt - M*Xttm1)
        # update state covariance
        St = (np.identity(2)-Kt*M)*Sttm1

        Y_pred.append(Xt[0][0])

    Y_pred = np.squeeze(np.asarray(Y_pred))
    # print(X_pred)

    plt.plot(np.linspace(0, T * len(pos), len(pos)), Y_pred, 'r')
    plt.plot(np.linspace(0, T * len(pos), len(pos)), pos, 'grey')
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.title(title)
    plt.show()


def kalman2D(Q, R, T, pos, title):
    M = np.mat([[1, 0, 0, 0], [0, 1, 0, 0]])
    PHI = np.mat([[1, 0, T, 0], [0, 1, 0, T], [0, 0, 1, 0], [0, 0, 0, 1]])
    print(str(pos[0][0]))
    Xt = np.mat([pos[0][0], pos[0][1], 0, 0]).getT()  # x_{t-1,t-1}
    St = np.identity(4)
    Y1_pred = []
    Y2_pred = []
    Q = np.mat([[0, 0, 0, 0], [0, 0, 0, 0], [0, 0, Q, 0], [0, 0, 0, Q]])
    R = np.mat([[R, 0], [0, R]])
    for val in range(len(pos)):
        # predict next state
        Xttm1 = PHI*Xt
        print("Xttml" + str(Xttm1))
        # predict next state covariance
        Sttm1 = PHI*St*PHI.getT() + Q
        print("Sttml" + str(Sttm1))
        # Obtain measurement
        Yt = pos[val]
        print("Yt" + str(Yt))
        # calculate Kalman gain
        Kt = Sttm1*M.getT()*np.mat(M*Sttm1*M.getT() + R).getI()
        print("Kt" + str(Kt))
        # update state
        Xt = Xttm1 + Kt*(Yt - M*Xttm1)
        print("Xt" + str(Xt))
        # update state covariance
        St = (np.identity(4)-Kt*M)*Sttm1
        print("St" + str(St))
        Y1_pred.append(Xt[0, 0])
        Y2_pred.append(Xt[0, 1])
        print()

    Y_pred = np.squeeze(np.asarray(Y1_pred))

    allData = np.array(pos)

    data1 = []
    data2 = []
    for val in range(len(allData)):
        data1.append(allData[val, 0])
        data2.append(allData[val, 1])
    # print(data1)
    # print(data2)
    plt.plot(np.linspace(0, T * len(data1), len(data1)), Y1_pred, 'r')
    plt.plot(np.linspace(0, T * len(data1), len(data1)), data1, 'grey')
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.title("2D data part1" + title)
    plt.show()

    plt.plot(np.linspace(0, T * len(data1), len(data1)), Y2_pred, 'r')
    plt.plot(np.linspace(0, T * len(data1), len(data1)), data2, 'grey')
    plt.xlabel('Time')
    plt.ylabel('Position')
    plt.title("2D data part2" + title)
    plt.show()


data_1D = readData("1D-data.txt", 1)
data_2D = readData("2D-data.txt", 2)

# constant velocity model
kalman1D(Q=.000001, R=1, T=1, pos=data_1D, title='1D-Velocity - Q=1E-6 R=1')
kalman1D(Q=.0000000001, R=1, T=1, pos=data_1D, title='1D-Velocity - Q=1E-10 R=1')
kalman1D(Q=.001, R=1, T=1, pos=data_1D, title='1D-Velocity - Q=1E-3 R=1')

# 2D model
kalman2D(Q=.01, R=1, T=1, pos=data_2D, title=' - Q=0.001 R=1')
