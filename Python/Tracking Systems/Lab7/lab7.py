from math import *


def viterbi_hmm(obs, states, init_prob, trans_prob, em_prob):
    Vit = []
    # calculate initial state probabilities (0 = H, 1 = L)
    Vit.append([init_prob[0]*em_prob[0][obs[0]], init_prob[1]*em_prob[1][obs[0]]])

    # calculate remaining state probabilities
    for i in range(1, len(obs)):
        # check both previous states
        H_prob = em_prob[states[0]][obs[i]] * max(Vit[i - 1][states[0]] * trans_prob[states[0]][states[0]],
                                                  Vit[i - 1][states[1]] * trans_prob[states[1]][states[0]])

        L_prob = em_prob[states[1]][obs[i]] * max(Vit[i - 1][states[0]] * trans_prob[states[0]][states[1]],
                                                  Vit[i - 1][states[1]] * trans_prob[states[1]][states[1]])

        Vit.append([H_prob, L_prob])

    best_path = []
    for HL in Vit:
        if isclose(HL[0], HL[1]):  # default to L if H ~= L
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
trans_prob = {
    0: {0: 0.5, 1: 0.5},
    1: {0: 0.4, 1: 0.6},
}
em_prob = {
    0: {0: 0.2, 1: 0.3, 2: 0.3, 3: 0.2},
    1: {0: 0.3, 1: 0.2, 2: 0.2, 3: 0.3},
}

[path, probs] = viterbi_hmm(obs1, states, init_state_prob, trans_prob, em_prob)
print_data(obs1, path, probs)

[path, probs] = viterbi_hmm(obs2, states, init_state_prob, trans_prob, em_prob)
print_data(obs2, path, probs)
