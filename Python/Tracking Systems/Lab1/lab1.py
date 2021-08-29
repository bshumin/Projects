import numpy as np
import matplotlib.pyplot as plt


# helper functions
def getXY(matrix, type):
    try:
        x_data = []
        y_data = []

        if type == 1:
            for val in matrix:
                x_data.append(val[0])
                y_data.append(val[1])
        elif type == 2:
            for val in matrix:
                x_data.append(val[2])
                y_data.append(val[3]/val[2])

        return [x_data, y_data]
    except:
        print("Failed to get X and Y coordinates. Check dimensions of matrix passed, it must be of Nx2 size.")
        exit()


def getNormalFuncFirstOrder(data, partnum):
    [x, y] = getXY(data, 1)

    # make A and B matrices
    mat_a = np.empty((0, 2), dtype=float)
    for i in range(len(x)):
        mat_a = np.append(mat_a, np.array([[x[i], 1]]), axis=0)

    mat_a = np.matrix(mat_a)
    mat_b = np.matrix(y, dtype=float).getT()

    # print(mat_a)
    # print()
    # print(mat_b)
    # print()

    # show data as it appears
    plt.scatter(x, y, marker='o')
    plt.grid(which='major')

    plt.xlabel('x-axis')
    plt.ylabel('y-axis')
    plt.title('Part ' + str(partnum))
    # plt.show()

    mat_x = (mat_a.getT() * mat_a)
    mat_x = np.linalg.inv(mat_x)
    mat_x = mat_x * mat_a.getT() * mat_b

    # print(mat_x)
    # print()

    # get form of y = m*x + b
    m = float(mat_x[0])
    b = float(mat_x[1])

    # print(m)
    # print(b)

    x2 = np.linspace(min(x), max(x), 100)
    y2 = m * x2 + b

    plt.plot(x2, y2, 'r')

    plt.show()
    plt.close()

def getNormalFuncInvX(data, partnum):
    [x, y] = getXY(data3, 2)

    # make A and B matrices
    mat_a = np.empty((0, 2), dtype=float)
    for i in range(len(x)):
        mat_a = np.append(mat_a, np.array([[1 / x[i], 1]]), axis=0)

    mat_a = np.matrix(mat_a)
    mat_b = np.matrix(y, dtype=float).getT()

    # print(mat_a)
    # print()
    # print(mat_b)
    # print()

    # show data as it appears
    plt.scatter(x, y, marker='.')
    plt.grid()

    plt.xlabel('# of Bites')
    plt.ylabel('Kcals/bite')
    plt.title('Part ' + str(partnum))

    mat_x = (mat_a.getT() * mat_a)
    mat_x = np.linalg.inv(mat_x)
    mat_x = mat_x * mat_a.getT() * mat_b

    # print(mat_x)
    # print()

    # get form of y = m*(1/x) + b
    m = float(mat_x[0])
    b = float(mat_x[1])

    # print(m)
    # print(b)

    x2 = np.linspace(0.5, max(x), 10000)
    y2 = m * (1 / x2) + b

    plt.plot(x2, y2, 'r')

    plt.show()
    plt.close()


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


# ====================================================== PART 1 ====================================================== #
data1 = [[5, 1],
         [6, 1],
         [7, 2],
         [8, 3],
         [9, 5]]

getNormalFuncFirstOrder(data1, 1)  # get normal function of data based on a first order linear regression

# ====================================================== PART 2 ====================================================== #
data2 = [[5, 1],
         [6, 1],
         [7, 2],
         [8, 3],
         [8, 14],  # added point to reduce model accuracy
         [9, 5]]


getNormalFuncFirstOrder(data2, 2)  # get normal function of data based on a first order linear regression

# ====================================================== PART 3 ====================================================== #

data3 = readData('data.txt')

getNormalFuncInvX(data3, 3)
