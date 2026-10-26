# Simplex Method


> A C implementation of the Revised Simplex Method (Two-Phase), developed as a practical project for the Linear Programming/Optimization course (MS428 - Unicamp).


![C Standard](https://img.shields.io/badge/C-C11-blue.svg)
![CMake](https://img.shields.io/badge/Build-CMake-orange.svg)
![CI/CD](https://img.shields.io/badge/GitHub%20Actions-Passing-brightgreen.svg)
![License](https://img.shields.io/badge/License-GPLv3-blue.svg )


---


## Authors
Vitor Aquino Mendonça de Siqueira\
Thierry Scarazzatti Adame\
Vinicius Silva Soares


---


## About the Project


This project solves linear programming optimization problems in standard form: 
$$\min c^T x \quad \text{s.t.} \quad Ax = b,\ x \ge 0$$
using the **Two-Phase Revised Simplex Method**. Built in C for a better computational performance and understanding of the memory and data management, it features robust gaussian elimination routines with partial and scaled pivoting to maintain numerical stability.


### Key Features
- **Full Two-Phase Algorithm:** Robust handling for finding an initial basic feasible solution and optimizing the objective function.
- **Flexible Input Formats:** Supports human-readable text files (`.txt`) for manual testing and high-performance binary files (`.bin`) parsed via `fread` for large-scale instances.
- **Configuration Hierarchy:** Command-line flags (`--text`, `--binary`), environment variables, and fallback defaults for input selection.
- **Safe Memory Management:** Defensive memory allocation utilizing the `goto cleanup` pattern to release memory allocation on every error.
- **Automated Testing & CI/CD:** GitHub Actions workflow running unit and integration tests automatically on every push.


---


## Tech Stack & Tools


- **Core Language:** C (C11 Standard)
- **Build System:** CMake
- **Unit Testing Framework:** `utest.h` (Single-header test library)
- **Test Generation & Plotting:** Python 3 (NumPy, Matplotlib)


---


##  Prerequisites


Ensure you have the following tools installed before building:
* C compiler with C11 support (`gcc` or `clang`)
* `CMake` (version 3.15 or higher)
* `Python 3.10+` with dependencies listed in `requirements.txt` (for benchmark scripts and graph generation)


---


## Project structure


```
.
├── .github/workflows/c-cpp.yml   CI workflow
├── CMakeLists.txt                Build, venv and fixture generation
├── main.c                        Entry point and argument parsing
├── initialize.c / .h             Input reading (text and binary)
├── phases_1_and_2.c / .h         Two-phase driver
├── simplex.c / .h                One simplex iteration and its helpers
├── gaussian_elimination.c / .h   Linear system solver
├── auxiliar_functions.c / .h     Index tracking and Phase I setup
├── test.c / .h                   Unit and integration tests (utest.h)
├── lp_test*.py                   Random LP generators for the tests
├── graph.py                      Benchmark and plots
├── input.txt                     Example input
└── requirements.txt              Python dependencies
```



## Building and Running
```bash
git clone [https://github.com/vitoraqdev/simplex-method.git](https://github.com/vitoraqdev/simplex-method.git)
mkdir build && cd build
cmake ..
make
```


Configuring also creates a python environment, and generates LP’s in binary format to feed test.c. This runs on every configure.
Building produces two executables in `build/`:

| Executable | Purpose |
|---|---|
| `SimplexMethod` | Solves the problem read from the input file |
| `SimplexTests` | Runs unit and integration tests |


## Input
### Input through .txt (default `input.txt`)
```
m n
c_1 ... c_n
A_11 ... A_1n
...
A_m1 ... A_mn
b_1 ... b_m
```


Example from the Revised Simplex Method article on Wikipedia:
```
2 5
-2 -3 -4 0 0
3 2 1 1 0
2 5 3 0 1
10 15
```


### Input through .bin (default `input.bin`)
All values are stored in the specific order

| Content | Type | Count |
|---|---|---|
| `m`, `n` | `int32` | 2 |
| `c` | `float64` | `n` |
| `A` (one row at a time) | `float64` | `m * n` |
| `b` | `float64` | `m` |

Test binaries can have one additional `float64` showing the optimal cost. `SimplexMethod` ignores this last number.


### Selecting the file and format
```
./build/SimplexMethod                                        # reads input.txt from source directory
./build/SimplexMethod --text path/to/myproblem.txt           # reads myproblem.txt 
./build/SimplexMethod --binary path/to/myproblem.bin         # reads myproblem.bin
./build/SimplexMethod --text                                 # reads LP_DATA_DIR/input.txt
./build/SimplexMethod --binary                               # reads LP_DATA_DIR/input.bin 
```
However you can pass the file you want to solve in multiple ways, and it follows a priority order:
1. Through command line
2. `LP_INPUT_PATH` environment variable
3. `LP_DATA_DIR/input.txt` or `LP_DATA_DIR/input.bin`
4. `input.txt` or `input.bin` in the source directory

The format can also be chosen by passing the environment variable `LP_INPUT_FORMAT = bin`, and otherwise is chosen as text.


## Output
The program prints for each phase the variables that optimize the linear problem. If at any point there is an error in the execution, it should interrupt and print the problem.
Again using the Wikipedia example, we have
```
Phase 1 variables:
x_0: 1.818182
x_1: 2.272727
x_2: 0
x_3: 0
x_4: 0
x_5: 0
x_6: 0


Phase 2 variables:
x_0: 0
x_1: 0
x_2: 5.000000
x_3: 5.000000
x_4: 0
Best cost: -20.000000
```
Phase 1 prints m+n variables, with the last m variables set to zero which are related to the artificials variables, phase 2 prints the original ones, both of them preserving the original order from input.
Errors include:
```
ERROR: First phase cost non-zero. Total cost {total_cost:.6}= 0.
ERROR: Max iteration ({MAX_ITERATION}) reached.
ERROR: No finite solution for the first (or second) phase.
ERROR: Not enough memory to allocate.
```

## Environment variables and build options


| Name | Set where | Meaning |
|---|---|---|
| `LP_INPUT_FORMAT` | environment, at run time | `bin` for binary input, otherwise text |
| `LP_INPUT_PATH` | environment, at run time | Explicit path to the input file |
| `LP_DATA_DIR` | environment, at run time | Directory where `input.txt` / `input.bin` is searched |
| `LP_TEST_M` | CMake option (`-DLP_TEST_M=...`) | Number of constraints $m$ of the generated test problem (default 100) |
| `LP_TEST_N` | CMake option (`-DLP_TEST_N=...`) | Number of variables $n$ of the generated test problem (default 200) |


`LP_TEST_M` and `LP_TEST_N` are set when configuring. CMake passes them to the Python generator as environment variables and to the C tests as compile definitions, so both sides always agree on the problem size.




## Tests
```bash
cmake --build build --target SimplexTests
cd build
./SimplexTests
```



Run it from `build/`, because the integration test opens its fixture through a relative path.


- **Unit tests:** `gaussian_elimination`, the simplex multiplier vector $\lambda$, reduced costs, entering variable, leaving variable and one `simplex_step`. They use the Wikipedia example, which already starts at the optimal basis, as an oracle.
- **Integration test:** solves a random feasible and bounded LP built by `lp_test.py` and compares the cost found with the expected optimal cost (tolerance `1e-3`). The generator constructs the problem so its optimum is known in advance, and shuffles the columns so the identity does not sit at the beginning of $A$. A fixed seed makes the problem identical on every run.

---

## Benchmark


`graph.py` generates problems of several sizes, runs `SimplexMethod` on each, stores the times in `results.json` (already finished sizes are not repeated) and plots execution time against $m$ and $n$.


```bash
python graph.py
```


---


## Continuous Integration


The workflow in `.github/workflows/c-cpp.yml` runs on every push and pull request to `main`. It checks out the repository, restores the cached Python virtual environment, installs the pinned CMake version, configures and builds `SimplexTests`, and runs it.


---



## References


- POLDI, Kelly Cristina. *Algoritmo Primal Simplex*. Lecture notes, MS428.
- Gordon College. *Gaussian Elimination* notes, <https://math.gordon.edu/courses/mat342/handouts/gauss.pdf>.
- Wikipedia. [Revised simplex method, numerical example](https://en.wikipedia.org/wiki/Revised_simplex_method#Numerical_example).
- [`utest.h`](https://github.com/sheredom/utest.h), single-header C testing framework.


