import numpy as np
import matplotlib.pyplot as plt
from math import *
import random as rand

plt.rcParams['font.size'] = '30'
plt.rcParams['font.family'] = 'serif'


# O - observation space
# S - state space
# PI - initial probabilities
# Y - Sequence of observations
# Tx - Transition matrix
# Em - Emission
# def viterbi_hmm(O, S, PI, Y, Tx, Em):
#     best_path = []
#     T1 = [[0 for _ in range(len(O))] for _ in range(len(S))]
# 
#     T2 = [[0 for _ in range(len(O))] for _ in range(len(S))]
#     for i in range(len(S)):
#         T1[i][1] = PI[i] * Em[i][O[0]]
#     for o in range(1, len(O)):
#         for s in range(len(S)):
#             kcomp = []
#             for val in range(len(T1)):
#                 kcomp.append(T1[val][o-1]*Tx[val][s]*Em[s][o])
#             # print(kcomp)
#             k = np.argmax(kcomp)
#             T1[s][o] = T1[k][o-1]*Tx[k][s]*Em[s][o]
#             T2[s][o] = k
#         k = np.argmax(T1)
#         for o in range(len(O)-1, -1, -1):
#             best_path.insert(0, S[k])
#             k = T2[k][o]
#     return best_path


def viterbi_hmm(obs, states, init_prob, tx_prob, em_prob):
    Vit = []
    # calculate initial state probabilities (0 = H, 1 = L)
    Vit.append([init_prob[0]*em_prob[0][obs[0]], init_prob[1]*em_prob[1][obs[0]]])

    # calculate remaining state probabilities
    prev_state = -1
    max_tr = 0
    for i in range(1, len(obs)):
        # check both previous states
        H_prob = em_prob[states[0]][obs[i]] * max(Vit[i - 1][states[0]] * tx_prob[states[0]][states[0]], Vit[i - 1][states[1]] * tx_prob[states[1]][states[0]])

        L_prob = em_prob[states[1]][obs[i]] * max(Vit[i - 1][states[0]] * tx_prob[states[0]][states[1]], Vit[i - 1][states[1]] * tx_prob[states[1]][states[1]])

        Vit.append([H_prob, L_prob])

    best_path = []
    for HL in Vit:
        if isclose(HL[0], HL[1]):  # default to L
            best_path.append(1)
        elif HL[0] > HL[1]:
            best_path.append(0)
        else:
            best_path.append(1)
    return [best_path, Vit]


def print_data(seq, path, data):
    obs_dec = {
        0: "A",
        1: "C",
        2: "G",
        3: "T"
    }
    states_dec = {
        0: "H",
        1: "L"
    }
    print("State\tH\t   L\tChosen State")
    for i in range(len(seq)):
        print("%c\t %0.2f\t%0.2f\t%c" % (obs_dec[seq[i]], log2(data[i][0]), log2(data[i][1]), states_dec[path[i]]))
    print()


obs1 = [2, 2, 1, 0, 1, 3, 2, 0, 0]
obs2 = [3, 1, 0, 2, 1, 2, 2, 1, 3]

states = [0, 1]
init_state_prob = {0: 0.5, 1: 0.5}
tx_prob = {
    0: {0: 0.5, 1: 0.5},
    1: {0: 0.4, 1: 0.6},
}
em_prob = {
    0: {0: 0.2, 1: 0.3, 2: 0.3, 3: 0.2},
    1: {0: 0.3, 1: 0.2, 2: 0.2, 3: 0.3},
}

[path, probs] = viterbi_hmm(obs1, states, init_state_prob, tx_prob, em_prob)
print_data(obs1, path, probs)

[path, probs] = viterbi_hmm(obs2, states, init_state_prob, tx_prob, em_prob)
print_data(obs2, path, probs)

