#tool to numerical aproximate a PT1 tranfer function
import numpy as np
from scipy.optimize import fsolve

# Define the function based on the given points
def equations(vars, x1, y1, x2, y2, x3, y3):
    a, x0, t1 = vars
    eq1 = y1 - (a - a * np.exp(-(x1 + x0) / t1))
    eq2 = y2 - (a - a * np.exp(-(x2 + x0) / t1))
    eq3 = y3 - (a - a * np.exp(-(x3 + x0) / t1))
    return [eq1, eq2, eq3]

# Initial guesses for a, x0, t1
initial_guess = [35.416269564093575, 575938.9233337401, 1981412.601652126]

# Points (x1, y1), (x2, y2), (x3, y3)
x1, y1 = 3317342, 25.017115
x2, y2 = 7032671, 28.60647
x3, y3 = 14361460, 30.30401

# Solve the system of equations
solution = fsolve(equations, initial_guess, args=(x1, y1, x2, y2, x3, y3))
a, x0, t1 = solution

print(f"a = {a}, x0 = {x0}, t1 = {t1}")