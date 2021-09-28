import numpy as np
import matplotlib.pyplot as plt
import sympy as sp
import math

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

# 2D constant velocity model
# (1) xt = [xt;yt;x't;y't]
# (2)
#   xt+1  = xt + Tx't
#   yt+1  = yt + Ty't
#   x't+1 = x't
#   y't+1 = y't
# (3) dynamix noise = [0;0;N(0, sigma(n1)^2);N(0, sigma(n2)^2)]
# (4) Yt = [x~t;y~t]
# (5)
#   x~t = xt
#   y~t = yt
# (6) measurement noise = [sigma(n1)^2);N(0, sigma(n2)^2)]
# (7) cov(state) = cov(x) =
#  NOTE: include cov(Q), cov(R) and PHI matrices in report as well


