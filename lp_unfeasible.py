import numpy as np

"""
Generates an unfeasible LP

min c^T*x
Ax = b
x >= 0

Outputs 3 binary files, one for each matrix to plug into the simplex testing.

We are going to generate A_{mxn}, b and c randomly, then copy one random row to another,
and add 1 to b_i to make sure it is very different, since b in [0, 1].

For testing purposes I'm going to permutate matrix A to remove the identity from the starting columns, as well as all
other variables. 
"""

import os
m = int(os.environ.get("LP_TEST_M", 100)) # default 100
n = int(os.environ.get("LP_TEST_N", 200)) # default 200
print(m, n)

RNG = np.random.default_rng(42)

A = RNG.random((m, n))
A[1, :m] = A[0, :m] # copy first -> second row
b = RNG.random(m)
b[1] = b[0] + 1 # make it unfeasible
c = RNG.random(n)


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
np.array([m], dtype=np.int32).tofile("lp_test_unfeasible_m.bin")
np.array([n], dtype=np.int32).tofile("lp_test_unfeasible_n.bin")
A.astype(np.float64).tofile("lp_test_unfeasible_A.bin")
b.astype(np.float64).tofile("lp_test_unfeasible_b.bin")
c.astype(np.float64).tofile("lp_test_unfeasible_c.bin")
