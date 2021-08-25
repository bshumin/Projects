import numpy as np
import scipy as sp
import scipy.stats as stats
import matplotlib.pyplot as plt


# helper functions
def getXY(matrix):
    try:
        x_data = []
        y_data = []

        for val in matrix:
            x_data.append(val[0])
            y_data.append(val[1])

        return [x_data, y_data]
    except:
        print("Failed to get X and Y coordinates. Check dimensions of matrix passed, it must be of Nx2 size.")
        exit()


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
mat1 = np.matrix(data1)  # matrix type for use in normal function

[x, y] = getXY(data1)

# show data as it appears
plt.scatter(x, y, marker='o')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Part 1')
plt.show()
plt.close()
stats.matrix_normal()
# ====================================================== PART 2 ====================================================== #
data2 = [[5, 1],
         [6, 1],
         [7, 2],
         [8, 3],
         [8, 14],  # added point to reduce model accuracy
         [9, 5]]
mat2 = np.matrix(data2)  # matrix type for use in normal function

[x, y] = getXY(data2)

# show data as it appears
plt.scatter(x, y, marker='o')

plt.xlabel('x-axis')
plt.ylabel('y-axis')
plt.title('Part 2')
plt.show()
plt.close()
# ====================================================== PART 3 ====================================================== #
print(readData('data.txt'))
