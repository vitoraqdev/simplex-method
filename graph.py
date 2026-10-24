import os
import subprocess
import time
import platform
import matplotlib.pyplot as plt

#Tuples (m, n) to test
tests = []

x_axis = []
y_axis = []
z_axis = []

for i in range(1, 1000, 10):
    for j in range(i, 1000, 10):
        tests.append((i, j))

binary_generator = "lp_test.py"

f = open("data.txt", 'w')

if platform.system() == "Windows":
    test_in_c = "./build/SimplexTests.exe"
else:
    test_in_c = "./build/SimplexTests"

for m, n in tests:
    
    #configures the environment variables and runs the generator script
    env_actual = os.environ.copy()
    env_actual["LP_TEST_M"] = str(m)
    env_actual["LP_TEST_N"] = str(n)
    
    subprocess.run(["python3", binary_generator], env=env_actual, check=True)
    
    # 2. Executes the C
    begin = time.time()
    result_c = subprocess.run([test_in_c], capture_output=True, text=True)
    end = time.time()
    time_taken = end - begin
    
    
    # if error, show it and break the loop
    if result_c.stderr:
        print("Errors:", result_c.stderr)
        break
    else:
        # Write the results to the file
        f.write(f"{m} {n} {time_taken}\n")
        f.flush()  # Ensure data is written to disk
        x_axis.append(n)
        y_axis.append(m)
        z_axis.append(time_taken)

f.close()


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

ax.set_xlabel('Número de Variáveis (n)')
ax.set_ylabel('Número de Restrições (m)')
ax.set_zlabel('Tempo de Execução (s)')
ax.set_title('Desempenho do Algoritmo Simplex')

ax.legend()
plt.show()