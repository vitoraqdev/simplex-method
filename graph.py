import os
import subprocess
import time
import platform
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor, as_completed
import json
import sys

tests = []

for i in range(1, 1000, 10):
    for j in range(i, 1000, 10):
        tests.append((i, j))

# reads dict json file, if does not exist, create new one
# if exists, only define values to keys not defined yet
RESULTS_FILE = "results.json"
TEMP_DIR = "build/temp"
BINARY_GENERATOR = "lp_test.py"
if platform.system() == "Windows":
    SIMPLEX_C = "./build/SimplexMethod.exe"
else:
    SIMPLEX_C = "./build/SimplexMethod"


def key(m, n):
    return f"{m},{n}"

def load_results(path=RESULTS_FILE):
    if not os.path.exists(path):
        return {}
    try:
        with open(path) as f:
            return json.load(f)
    except json.JSONDecodeError:
        # corrupted file (e.g. killed mid-write): don't silently overwrite it
        os.replace(path, path + ".corrupt")
        return {}

def save_results(results, path=RESULTS_FILE):
    # write to a temp file, then atomically replace, so a crash never leaves half a JSON
    tmp = path + ".tmp"
    with open(tmp, "w") as f:
        json.dump(results, f)
    os.replace(tmp, path)


#function to run a single test
def run_single_test(m, n):
    file_name = f"input_{m}_{n}.bin"
    file_path = os.path.join(TEMP_DIR, file_name)
    try:
        #configures the environment variables and runs the generator script
        env_actual = os.environ.copy()
        env_actual["LP_TEST_M"] = str(m)
        env_actual["LP_TEST_N"] = str(n)


        subprocess.run([sys.executable, BINARY_GENERATOR, file_name, TEMP_DIR],
            env=env_actual,
            check=True,
            capture_output=True)

        #executes the test and measures the time taken
        begin = time.perf_counter()
        result_c = subprocess.run([SIMPLEX_C, "--binary", file_path],
            capture_output=True,
            text=True)
        elapsed = time.perf_counter() - begin

        if result_c.returncode != 0:
            return m, n, None, result_c.stderr

        return m, n, elapsed, None
    except Exception as e:
        return m, n, None, str(e)
    finally:
        if os.path.exists(file_path):
            os.remove(file_path) # remove used binary

#open data file for writing
if __name__ == "__main__":
    results = load_results()
    pending_tests = [test for test in tests if f"{test[0]},{test[1]}" not in results]
    print(f"{len(results)} cached, {len(pending_tests)} pending tests")
    done = 0
    print("Starting parallel tests...")

    with ProcessPoolExecutor() as executor:
        futures = {executor.submit(run_single_test, *test): test for test in pending_tests}

        for future in as_completed(futures):
            m, n, time_taken, error = future.result()

            if error:
                print("Errors:", error)
                continue

            #write the results in the data file
            results[key(m, n)] = time_taken
            # every 20 write to results.json
            done += 1
            if done % 20 == 0:
                save_results(results, RESULTS_FILE)

    save_results(results, RESULTS_FILE)

    print("Tests completed! Generating plot...")

    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')

    x_axis, y_axis, z_axis = [], [], []
    for k, t in results.items():
        m, n = map(int, k.split(","))
        x_axis.append(n)
        y_axis.append(m)
        z_axis.append(t)

    ax.scatter(
        x_axis, y_axis, z_axis,
        c='r',
        marker='o',
        s=20,
        alpha=0.8,
        label='Tempo de Execução para (m,n)'
    )

    ax.set_xlabel('Nuúmero de Variáveis (n)')
    ax.set_ylabel('Número de Restrições (m)')
    ax.set_zlabel('Tempo de Execução (s)')
    ax.set_title('Desempenho do Algoritmo Simplex')

    ax.legend()
    plt.show()