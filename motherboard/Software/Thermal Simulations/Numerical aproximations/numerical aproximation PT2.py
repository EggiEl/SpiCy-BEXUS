import numpy as np
from scipy.optimize import fsolve

K = 35.4162695640935
# Define the function based on the given points
def equations(vars, x1, y1, x2, y2, x3, y3):
 
    T1, T2, x0 = vars
    eq1 = y1 - K * (1 - (T1 * np.exp(-(x1 + x0) / T1) - T2 * np.exp(-(x1 + x0) / T2)) / (T1 - T2))
    eq2 = y2 - K * (1 - (T1 * np.exp(-(x2 + x0) / T1) - T2 * np.exp(-(x2 + x0) / T2)) / (T1 - T2))
    eq3 = y3 - K * (1 - (T1 * np.exp(-(x3 + x0) / T1) - T2 * np.exp(-(x3 + x0) / T2)) / (T1 - T2))
    # eq4 = y4 - K * (1 - (T1 * np.exp(-(x4 + x0) / T1) - T2 * np.exp(-(x4 + x0) / T2)) / (T1 - T2))
    return [eq1, eq2, eq3]


# Initial guesses for T1, T2, x0
initial_guess = [395, 2715, 995]

# Points (x1, y1), (x2, y2), (x3, y3)
x1, y1 = 2724, 8.969721
x2, y2 = 324948, 12.939051
x3, y3 =  608731, 15.761957
x4, y4 =2116346, 26.315119

# Solve the system of equations
solution = fsolve(equations, initial_guess, args=(x1, y1, x2, y2, x3, y3))
T1, T2, x0 = solution

print(f"K = {K}, T1 = {T1}, T2 = {T2}, x0 = {x0}")
