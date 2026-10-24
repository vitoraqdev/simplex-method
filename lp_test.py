import numpy as np

"""
Generates a feasible and bounded LP problem

min c^T*x
Ax = b
x >= 0

Outputs 4 binary files, one for each matrix and one for the optimal solution, to plug into the simplex testing.

We are going to generate A_{mxn} and x_optimal randomly, and force the first m columns of A to be the identity,
then solve b = A*x, and since we want x_optimal to be the solution, reduced cost must be >= 0 (minimization problem).
=>  ĉ_Nj = c_Nj - λ^T * a_Nj >= 0
=>  c_Nj >= λ^T * a_Nj
=>  c_Nj = λ^T * a_Nj + slack,  slack >= 0
Since B = I_m
=>  c_Nj = c_B^T * B^-1 * a_Nj + slack
=>  c_Nj = c_B^T * a_Nj + slack
=>  c_N = c_B^T * N + slack

So we can just generate c_B and a slack for each j, and calculate c_N so that x_optimal is optimal.
Therefore we have a feasible bounded LP.

Optimal cost is
    c_B * x_B = c_B * b 

For testing purposes I'm going to permutate matrix A to remove the identity from the starting columns, as well as all
other variables. 
"""

import os
m = int(os.environ.get("LP_TEST_M", 100)) # default 100
n = int(os.environ.get("LP_TEST_N", 200)) # default 200

RNG = np.random.default_rng(42)

A = RNG.random((m, n))

A[:m, :m] = np.eye(m)

x_optimal = RNG.random(n)
b = A @ x_optimal
c_B = RNG.random(m)
slack = RNG.random(n-m)

c_N = c_B @ A[:, m:] + slack

c = np.concatenate((c_B, c_N))

optimal_cost = c_B @ b


"""
Permutate resetting the seed, to have same permutation for all matrices
"""

RNG = np.random.default_rng(42)
RNG.shuffle(A, axis=1)
RNG = np.random.default_rng(42)
RNG.shuffle(c)

# print(A[:m, :m])
# print(c[:m])
# print(optimal_cost)
# Output to bin file
m.astype(np.float64).tofile("lp_test_m.bin")
n.astype(np.float64).tofile("lp_test_n.bin")
A.astype(np.float64).tofile("lp_test_A.bin")
b.astype(np.float64).tofile("lp_test_b.bin")
c.astype(np.float64).tofile("lp_test_c.bin")
optimal_cost.astype(np.float64).tofile("lp_test_obj.bin")
