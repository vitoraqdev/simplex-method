import os
import subprocess
import time
import platform
import matplotlib.pyplot as plt
from concurrent.futures import ProcessPoolExecutor, as_completed

#tuples (m, n) to test
tests = []

x_axis = []
y_axis = []
z_axis = []

for i in range(1, 1000, 10):
    for j in range(i, 1000, 10):
        tests.append((i, j))

binary_generator = "lp_test.py"

if platform.system() == "Windows":
    test_in_c = "./build/SimplexTests.exe"
else:
    test_in_c = "./build/SimplexTests"

#function to run a single test
def run_single_test(pair):
    m, n = pair
    try:
        #configures the environment variables and runs the generator script
        env_actual = os.environ.copy()
        env_actual["LP_TEST_M"] = str(m)
        env_actual["LP_TEST_N"] = str(n)
        
        subprocess.run(["python3", binary_generator], env=env_actual, check=True, capture_output=True)
        
        #executes the test and measures the time taken
        begin = time.time()
        result_c = subprocess.run([test_in_c], capture_output=True, text=True)
        end = time.time()
        time_taken = end - begin
        
        if result_c.returncode != 0:
            return m, n, None, result_c.stderr
            
        return m, n, time_taken, None
    except Exception as e:
        return m, n, None, str(e)

#open data file for writing
with open("data.txt", 'w') as f:
    print("Starting parallel tests...")
    
    with ProcessPoolExecutor() as executor:
        future_to_test = {executor.submit(run_single_test, test): test for test in tests}
        
        for future in as_completed(future_to_test):
            m, n, time_taken, error = future.result()
            
            #if error, show it and break the loop
            if error:
                print("Errors:", error)
                break
            else:
                #write the results in the data file
                f.write(f"{m} {n} {time_taken}\n")
                f.flush()  #ensure data is in disk
                x_axis.append(n)
                y_axis.append(m)
                z_axis.append(time_taken)

print("Tests completed! Generating plot...")

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

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