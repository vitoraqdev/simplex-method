import numpy as np

"""
Generates a feasible and unbounded LP problem

min c^T*x
Ax = b
x >= 0

Outputs 3 binary files, one for each matrix, to plug into the simplex testing.

A problem is unfeasible if for any k-th non-basic variable with reduced cost < 0, direction <= 0.
We are going to generate A_{mxn} and x_optimal randomly, and force the first m columns of A to be the identity.

For direction y <= 0, since By = a_N0, then y = a_N0, so as long as a_N0 <= 0. 

pick one non-basic variable (the first one), and force reduced cost < 0:
=>  ĉ_N0 = c_N0 - λ^T * a_N0 < 0
=>  c_N0 < λ^T * a_N0
=>  c_N0 < c_B^T * B^-1 * a_N0
=>  c_N0 < c_B^T * a_N0
=>  c_N0 := c_B^T * a_N0 - slack, slack > 0
Selecting slack as 1, we only need to generate c, and set c_N0 as c_B^T * a_N0 - 1, and we are done,
therefore we have a feasible unbounded LP.

For testing purposes I'm going to permutate matrix A to remove the identity from the starting columns, as well as all
other variables. 
"""

import os
m = int(os.environ.get("LP_TEST_M", 100)) # default 100
n = int(os.environ.get("LP_TEST_N", 200)) # default 200
print(m, n)

RNG = np.random.default_rng(42)

A = RNG.random((m, n))

A[:m, :m] = np.eye(m)
b = RNG.random(m)
c = RNG.random(n)


slack = 1
A[:, m] = -A[:, m] # since random generates [0, 1], invert column sign
c_B = c[:m]
c[m] = c_B.T @ A[:, m] - slack # c_N0

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
np.array([m], dtype=np.int32).tofile("lp_test_unbounded_m.bin")
np.array([n], dtype=np.int32).tofile("lp_test_unbounded_n.bin")
A.astype(np.float64).tofile("lp_test_unbounded_A.bin")
b.astype(np.float64).tofile("lp_test_unbounded_b.bin")
c.astype(np.float64).tofile("lp_test_unbounded_c.bin")
