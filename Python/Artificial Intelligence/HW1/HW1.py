import numpy as np
import matplotlib.pyplot as plt
import random as rand


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
    for row in data:
        for i in range(len(row)):
            if row[i] > feature_maxs[i]:
                feature_maxs[i] = row[i]

            if row[i] < feature_mins[i]:
                feature_mins[i] = row[i]

    # normalize data
    for row in data:
        for i in range(len(row)):
            row[i] = (row[i] - feature_mins[i]) / (feature_maxs[i] - feature_mins[i])
            # new_data.append(row)

    return data


def show_norm_plots(norm_features, norm_target):
    fig, axs = plt.subplots(4, 4)
    axs[0, 0].plot(norm_features[:, 0], norm_target, 'o')
    axs[0, 1].plot(norm_features[:, 1], norm_target, 'o')
    axs[0, 2].plot(norm_features[:, 2], norm_target, 'o')
    axs[0, 3].plot(norm_features[:, 3], norm_target, 'o')
    axs[1, 0].plot(norm_features[:, 4], norm_target, 'o')
    axs[1, 1].plot(norm_features[:, 5], norm_target, 'o')
    axs[1, 2].plot(norm_features[:, 6], norm_target, 'o')
    axs[1, 3].plot(norm_features[:, 7], norm_target, 'o')
    axs[2, 0].plot(norm_features[:, 8], norm_target, 'o')
    axs[2, 1].plot(norm_features[:, 9], norm_target, 'o')
    axs[2, 2].plot(norm_features[:, 10], norm_target, 'o')
    axs[2, 3].plot(norm_features[:, 11], norm_target, 'o')
    axs[3, 0].plot(norm_features[:, 12], norm_target, 'o')
    axs[3, 1].plot(norm_features[:, 13], norm_target, 'o')
    axs[3, 2].plot(norm_features[:, 14], norm_target, 'o')

    fig.delaxes(axs[3, 3])

    plt.show()


# function to split data into training and validation sets
def split_dataset(features, targets, train_ratio=.8):
    # create array of feature indices to randomly select from
    selection = np.array(range(len(features)))

    # randomize selection data
    rand.shuffle(selection)

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


def make_weights(x, y):

    n = len(x)
    w = []
    for wi in range(-10000, 10000):
        sum1 = 0
        for i in range(n):
            sum1 += abs(x[i]*wi - y[i])
        sum1 *= (1/n)
        w.append(sum1)
    print(w)
    min_w = min(w)
    print(min(w))

    return True


def predict(w, xi):
    # get prediction function
    return np.cross(w, xi)


def loss_func(fx, y, w, lamda):

    sum1 = 0
    for ele1, ele2 in fx, y:
        sum1 += pow(ele1 - ele2, 2)
    sum1 *= 1 / (2 * len(fx))

    sum2 = 0
    for ele in w:
        sum2 += pow(ele, 2)
    sum2 *= lamda/(2 * len(w))

    jw = sum1 + sum2

    return jw


def minimize_loss(fx, x, y, lamda, w):

    sum = 0
    wj_arr = []
    for wj in w:
        for ele1, ele2, ele3 in fx, x, y:
            sum += (ele1 - ele3) * ele2 + (lamda/(len(w)+1))*wj
        wj_arr.append(sum)


# learning rate parameters
alpha = 0.01
lambda_r = 2  # lambda value for ridge regularization
lambda_l = 0.3  # lambda value for lasso regularization

# get data and normalize
full_data = read_data()
norm_data = normalize_data(full_data)
norm_data = np.array(norm_data)

# split target data from features
[norm_features, norm_target] = norm_data[:, 0:-1], norm_data[:, -1]

# append column of ones to features
one = np.ones((len(norm_features), 1))
norm_features = np.append(norm_features, one, axis=1)
norm_target = np.array(norm_target).reshape(len(norm_target), 1)

show_norm_plots(norm_features, norm_target)

# split dataset into training and validation based on training ratio (defaults to 0.8-0.2 training to validation)
[tf, tt, vf, vt] = split_dataset(norm_features, norm_target)

# get linear regression weights
weights = np.linalg.inv(tf.T.dot(tf)).dot(tf.T).dot(tt)
weights = np.transpose(weights)
# weights = make_weights(tf, tt)
tf = np.transpose(tf)

fx = [predict(weights, ele) for ele in tf]
print(fx)



