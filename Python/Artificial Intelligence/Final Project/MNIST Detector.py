import os
import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from random import randrange

from matplotlib import pyplot as plt
import PIL
import cv2

import torch
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
from torch import nn

train_df = pd.read_csv("../Final Project/train.csv")
#test_df = pd.read_csv("../input/digit-recognizer/test.csv")

train_df, val_df = train_test_split(train_df, test_size=0.2, random_state=42)

def show_img_from_df(df, ind):
    plt.imshow(np.reshape(np.array(df.iloc[ind,1:]), (28,28)), cmap="gray")

show_img_from_df(train_df, 2)

new_size = 90

df = train_df
ind = randrange(1000)

img = np.reshape(np.array(df.iloc[ind, 1:]), (28, 28))


new_img = np.zeros((new_size, new_size))

# randomly select a bottom left corner to use for img
x_min, y_min = randrange(new_size - img.shape[0]), randrange(new_size - img.shape[0])
x_max, y_max = x_min + img.shape[0], y_min + img.shape[0]

x_center = x_min + (x_max-x_min)/2
y_center = y_min + (y_max-y_min)/2


new_img[x_min:x_max, y_min:y_max] = img
new_img = cv2.rectangle(new_img, (y_max, x_min), (y_min, x_max), 255, 1)
plt.imshow(new_img, cmap="gray")

plt.plot(y_center, x_center, "og", markersize=10)
plt.show()


class CustomMnistDataset_OL(Dataset):

    def __init__(self, df, test=False):
        '''
        df is a pandas dataframe with 28x28 columns for each pixel value in MNIST
        '''
        self.df = df
        self.test = test

    def __len__(self):
        return len(self.df)

    def __getitem__(self, idx):
        if self.test:
            image = np.reshape(np.array(self.df.iloc[idx, :]), (28, 28)) / 255.
        else:
            image = np.reshape(np.array(self.df.iloc[idx, 1:]), (28, 28)) / 255.

        # create the new image
        new_img = np.zeros((90, 90))  # images will be 90x90
        # randomly select a bottom left corner to use for img
        x_min, y_min = randrange(90 - image.shape[0]), randrange(90 - image.shape[0])
        x_max, y_max = x_min + image.shape[0], y_min + image.shape[0]

        x_center = x_min + (x_max - x_min) / 2
        y_center = y_min + (y_max - y_min) / 2

        # try normalizing this part of the output
        x_center = x_center  # / 90.
        y_center = y_center  # / 90.

        new_img[x_min:x_max, y_min:y_max] = image

        # NEW
        new_img = np.reshape(new_img, (1, 90, 90))

        label = [int(self.df.iloc[idx, 0]), np.array([x_center, y_center]).astype(
            'float32')]  # the label consists of the digit and the center of the number
        sample = {"image": new_img, "label": label}

        return sample['image'], sample['label']

trainingData = CustomMnistDataset_OL(train_df)
valData = CustomMnistDataset_OL(val_df)

train_dataloader = DataLoader(trainingData, batch_size=64, shuffle=True)
val_dataloader = DataLoader(valData, batch_size=64, shuffle=True)

cx = next(iter(train_dataloader))

indx = randrange(63)

plt.imshow(np.reshape(cx[0][indx], (90,90)))
print("Image shape: " + str(list(cx[0][indx].shape)))
print("Digit: " + str(int(cx[1][0][indx])))
print("Center: ({},{})".format(str(int(cx[1][1][indx][0])), str(int(cx[1][1][indx][1]))))

# Get cpu or gpu device for training.
device = "cuda" if torch.cuda.is_available() else "cpu"
print("Using {} device".format(device))


class NeuralNetwork_OL_v2(nn.Module):
    '''
    New convolutional model (v2)
    '''

    def __init__(self):
        super(NeuralNetwork_OL_v2, self).__init__()

        self.conv0 = nn.Conv2d(1, 16, 3, padding=(2, 2))  # 3x3 filters w/ same padding
        self.pool0 = nn.MaxPool2d(2, stride=2)
        self.conv1 = nn.Conv2d(16, 16, 3, padding=(3, 3))  # 3x3 filters w/ same padding
        self.pool1 = nn.MaxPool2d(2, stride=2)
        self.flatten = nn.Flatten()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(16 * 25 * 25, 256),
            nn.ReLU(),
            #             nn.Dropout(p=0.2),
            #             nn.Linear(256, 256),
            #             nn.ReLU(),
            #             nn.Dropout(p=0.5),
            #             nn.Linear(256, 256),
            #             nn.ReLU()
        )
        self.linear = nn.Linear(256, 10)
        self.linear_x = nn.Linear(256, 1)
        self.linear_y = nn.Linear(256, 1)
        self.linear_all = nn.Linear(256, 2)

    def forward(self, x):
        x = self.conv0(x)
        #         print(x.shape)
        x = F.relu(self.pool0(x))
        #         print(x.shape)
        x = self.conv1(x)
        #         print(x.shape)
        x = F.relu(self.pool1(x))
        #         print(x.shape)
        x = self.flatten(x)
        x = self.linear_relu_stack(x)
        logits = self.linear(x)
        #         x_cent = self.linear_x(x)
        #         y_cent = self.linear_y(x)
        centr = self.linear_all(x)
        return logits, centr  # logits, x_cent, y_cent


model = NeuralNetwork_OL_v2().to(device)
print(model)

loss_fn = nn.CrossEntropyLoss()
loss_mse = nn.MSELoss()
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)#torch.optim.SGD(model.parameters(), lr=1e-3)
alpha = 100
beta = 1


def train(dataloader, model, loss_fn, loss_mse, optimizer, alpha, beta):
    model.train()  # very important... This turns the model back to training mode
    size = len(train_dataloader.dataset)

    loss_dig_list = []
    loss_center_list = []

    for batch, (X, y) in enumerate(dataloader):

        X, y0, y1 = X.to(device), y[0].to(device), y[1].to(device)

        y0_pred, y1_pred = model(X.float())

        loss = alpha * loss_fn(y0_pred, y0) + beta * loss_mse(y1_pred, y1.float())
        loss_dig = loss_fn(y0_pred, y0)
        loss_center = loss_mse(y1_pred, y1.float())

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        if batch % 100 == 0:
            loss, current = loss.item(), batch * len(X)

            loss_dig = loss_dig.item()
            loss_center = loss_center.item()

            loss_dig_list.append(loss_dig)
            loss_center_list.append(loss_center)

            print(f"MAIN loss: {loss:>7f}  [{current:>5d}/{size:>5d}]")
            print(f"Digit prediction loss: {loss_dig:>7f}  [{current:>5d}/{size:>5d}]")
            print(f"Coordinate prediction loss: {loss_center:>7f}  [{current:>5d}/{size:>5d}]")
            print("-----------")


# TODO: make this work for three outputs....

def test(dataloader, model, loss_fn, loss_mse, alpha=alpha, beta=beta):
    size = len(dataloader.dataset)
    model.eval()
    test_loss, test_loss_y0, test_loss_y1, correct = 0, 0, 0, 0
    with torch.no_grad():
        for X, y in dataloader:
            X, y0, y1 = X.to(device), y[0].to(device), y[1].to(device)
            y0_pred, y1_pred = model(X.float())
            test_loss += alpha * loss_fn(y0_pred, y0).item() + beta * loss_mse(y1_pred, y1.float()).item()
            test_loss_y0 += loss_fn(y0_pred, y0).item()
            test_loss_y1 += loss_mse(y1_pred, y1.float()).item()

            correct += (y0_pred.argmax(1) == y0).type(torch.float).sum().item()  # only for digit predictions

    # average the loss and accuracy among all records used in the dataset
    test_loss /= size
    test_loss_y0 /= size
    test_loss_y1 /= size
    correct /= size
    print(
        f"Test Error: \n Accuracy: {(100 * correct):>0.1f}%, Avg digit loss: {test_loss_y0:>8f}, Avg coordinate loss: {test_loss_y1:>8f} \n")

epochs = 1
for t in range(epochs):
    print(f"Epoch {t+1}\n-------------------------------")
    train(train_dataloader, model, loss_fn, loss_mse, optimizer, alpha=alpha, beta=beta)
    test(val_dataloader, model, loss_fn, loss_mse, alpha=alpha, beta=beta)
print("Done!")

(X, y) = next(iter(val_dataloader))

indx = randrange(63)
y_center_actual, x_center_actual = int(y[1][indx][0]), int(y[1][indx][1])

digit_pred, center_pred = model(X.to(device).float())

predicted_digit = np.argmax(digit_pred[indx].cpu().detach().numpy())

predicted_x = int(center_pred[indx][0])
predicted_y = int(center_pred[indx][1])

indx = randrange(63)
y_center_actual, x_center_actual = int(y[1][indx][0]), int(y[1][indx][1])

digit_pred, center_pred = model(X.to(device).float())

predicted_digit = np.argmax(digit_pred[indx].cpu().detach().numpy())

predicted_x = int(center_pred[indx][0])
predicted_y = int(center_pred[indx][1])


plt.imshow(np.reshape(X[indx].cpu().numpy(), (90,90)), cmap="gray")
# plot the actual center in green
plt.plot(x_center_actual, y_center_actual, "og", markersize=10)
# plot the predicted center in orange
plt.plot(predicted_y, predicted_x, "oy", markersize=10)
plt.show()

print("Image shape: " + str(list(X[indx].cpu().numpy().shape)))
print("Digit: " + str(int(y[0][indx])))
print("True Center (in green): ({},{})".format(y_center_actual, x_center_actual))
print("-------------------------------")
print("Predicted Digit: "+str(predicted_digit))
print("Predicted Center (in yellow): ({},{})".format(str(predicted_x), str(predicted_y)))