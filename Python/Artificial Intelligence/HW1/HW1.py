import numpy as np
import matplotlib.pyplot as plt
import random as rand
from math import sqrt


# function to read data file and format data into usable format
def read_data(filename="data.txt"):
    with open(filename) as data_file:
        # get number of rows and columns
        cols = int(data_file.readline().split(' ')[0])
        rows = int(data_file.readline().split(' ')[0])

        # skip through header data
        for _ in range(cols):
            _ = data_file.readline()

        all_data = []
        for _ in range(rows):
            # split data and throw away any erroneous spaces, then convert all data to floats. Also ignore index
            data = data_file.readline().replace('\n', '').split(' ')
            data = [float(i) for i in data[2:] if i != '' and i != ' ']

            # append formatted data to list of all data
            all_data.append(data)

    return all_data


def normalize_data(data):
    feature_mins = [data[0][0] for _ in data[0]]
    feature_maxs = [data[0][0] for _ in data[0]]

    # get minimums and maxes for for all features
    # for row in data:
    #     for i in range(len(row)):
    #         if row[i] > feature_maxs[i]:
    #             feature_maxs[i] = row[i]
    #
    #         if row[i] < feature_mins[i]:
    #             feature_mins[i] = row[i]
    for i in range(len(data[0])):
        feature_mins[i] = np.min(data[:, i])
        feature_maxs[i] = np.max(data[:, i])

    # normalize data
    for row in data:
        for i in range(len(row)):
            row[i] = (row[i] - feature_mins[i]) / (feature_maxs[i] - feature_mins[i])
            # new_data.append(row)
    return data


# function to split data into training and validation sets
def split_dataset(features, targets, train_ratio=.8):
    # create array of feature indices to randomly select from
    selection = np.array(range(len(features)))

    # randomize selection data
    # rand.shuffle(selection)

    # split sets based on training ratio
    training_set = selection[0:round(train_ratio * len(features))]
    validation_set = selection[round(train_ratio * len(features)):len(features)]

    # get training sets
    train_features = features[training_set, :]
    train_target = targets[training_set, :]

    # get validation sets
    validation_features = features[validation_set, :]
    validation_targets = targets[validation_set, :]

    return [train_features, train_target, validation_features, validation_targets]


# classical regression formula implementation
def regression(w, xi):
    return np.dot(w.T, xi)


# function to perform ridge regression
def ridge_regression(Xbar, y, test_x, test_y, lambda_val, alpha=0.01, epsilon=0.001):
    # get sample and feature sizes
    sample, feature = Xbar.shape
    test_sample, _ = test_x.shape

    # initialize weights
    w = np.zeros(feature,)

    # initialize list of all losses ans MSE
    all_Jw = []
    all_MSE = []

    # initialize loop condition and iteration count
    delta = 1
    iterations = 0

    while delta > epsilon:
        # calculate loss function
        Jw1 = 0
        for i in range(sample):
            Jw1 += sum((regression(w, Xbar[i, :]) - y[i])**2)
        Jw1 *= (1 / (2 * sample))

        Jw2 = 0
        for j in range(feature):
            Jw2 += w[j] ** 2
        Jw2 *= (lambda_val / (2 * feature))
        Jw = Jw1 + Jw2
        all_Jw.append(Jw)

        # minimize loss function via gradient descent
        djw = np.zeros(feature, )
        for j in range(feature):
            for i in range(sample):
                djw[j] += (regression(w, Xbar[i, :]) - y[i]) * Xbar[i, j]
            djw[j] *= 1 / sample
            djw[j] += (lambda_val / feature) * w[j]

        # update weights
        w = w - alpha * djw

        # calculate MSE
        MSE = 0
        for i in range(test_sample):
            MSE += (test_y[i]-regression(w, test_x[i, :]))**2
        MSE *= 1/(2*test_sample)
        all_MSE.append(MSE)

        # update loop-break condition if not the first iteration
        if len(all_Jw) > 1:
            delta = (abs(all_Jw[iterations-1]-all_Jw[iterations])*100)/all_Jw[iterations-1]
        iterations += 1  # increment iterations

    return w, all_Jw, all_MSE, iterations


# function to perform ridge regression
def lasso_regression(Xbar, y, test_x, test_y, lambda_val, alpha=0.01, epsilon=0.001):
    # get sample and feature sizes
    sample, feature = Xbar.shape
    test_sample, _ = test_x.shape

    # initialize weights
    w = np.zeros(feature, )

    # initialize list of all losses ans MSE
    all_Jw = []
    all_MSE = []

    # initialize loop condition and iteration count
    delta = 1
    iterations = 0

    while delta > epsilon:

        # calculate loss function
        Jw1 = 0
        for i in range(sample):
            Jw1 += (y[i]-regression(w, Xbar[i, :]))**2
        Jw1 *= 1/(2 * sample)

        Jw2 = 0
        for j in range(feature):
            Jw2 += abs(w[j])
        Jw2 *= lambda_val / (2 * feature)

        Jw = Jw1 + Jw2
        all_Jw.append(Jw)

        # minimize loss function via gradient descent
        djw = np.zeros(feature, )

        for j in range(feature):
            for i in range(sample):
                djw[j] += (y[i] - regression(w, Xbar[i, :])) * (-Xbar[i, j])
            djw[j] *= 1 / sample

            if w[j] == 0:
                djw[j] += (lambda_val / feature) * 1
            else:
                djw[j] += (lambda_val / feature) * (w[j])/sqrt(w[j]**2)

        # update weights
        w = w - alpha * djw

        # calculate MSE
        MSE = 0
        for i in range(test_sample):
            MSE += (test_y[i] - regression(w, test_x[i, :])) ** 2
        MSE *= 1 / (2 * test_sample)
        all_MSE.append(MSE)

        # update loop-break condition if not the first iteration
        if len(all_Jw) > 1:
            delta = abs(all_Jw[iterations - 1] - all_Jw[iterations]) * 100 / all_Jw[iterations - 1]
        iterations += 1  # increment iterations

    return w, all_Jw, all_MSE, iterations


def small_weights(w):
    count = 0
    for ele in weights:
        if abs(ele) < 0.01:
            count += 1
    return count

# learning rate parameters
alpha = 0.01
epsilon = 0.001
lambda_r = 2  # lambda value for ridge regularization
lambda_l = 0.3  # lambda value for lasso regularization

# get data and normalize
full_data = read_data()
full_data = np.array(full_data)
norm_data = normalize_data(full_data)
# norm_data = np.array(norm_data)

# split target data from features
[norm_features, norm_target] = norm_data[:, 0:-1], norm_data[:, -1]
# append column of ones to features
one = np.ones((len(norm_features), 1))
norm_features = np.append(norm_features, one, axis=1)
norm_target = np.array(norm_target).reshape(len(norm_target), 1)

# split dataset into training and validation based on training ratio (defaults to 0.8-0.2 training to validation)
[tf, tt, vf, vt] = split_dataset(norm_features, norm_target)

[weights, losses, mse, iterations] = ridge_regression(tf, tt, vf, vt, lambda_r, alpha=alpha, epsilon=epsilon)

# plot loss and MSE against iterations
plt.plot(range(iterations), losses)
plt.plot(range(iterations), mse)
plt.title('Ridge Regression Gradient Descent')
plt.xlabel('Iterations')
plt.ylabel('J$_k$(w)/MSE')
plt.legend(['J$_k$(w)', 'MSE'])
plt.show()

# get number of weights under 0.01
count = small_weights(weights)
print('Ridge Regression')
print("Number of elements in w less than 0.01: " + str(count))
print('Final loss: ' + str(losses[-1]))
print('Final MSE: ' + str(mse[-1][0]) + '\n')

[weights, losses, mse, iterations] = lasso_regression(tf, tt, vf, vt, lambda_l, alpha=alpha, epsilon=epsilon)

# plot loss and MSE against iterations
plt.plot(range(iterations), losses)
plt.plot(range(iterations), mse)
plt.title('Lasso Regression Gradient Descent')
plt.xlabel('Iterations')
plt.ylabel('J$_k$(w)/MSE')
plt.legend(['J$_k$(w)', 'MSE'])
plt.show()

# get number of weights under 0.01
count = small_weights(weights)
print('Lasso Regression')
print("Number of elements in w less than 0.01: " + str(count))
print('Final loss: ' + str(losses[-1][0]))
print('Final MSE: ' + str(mse[-1][0]) + '\n')
