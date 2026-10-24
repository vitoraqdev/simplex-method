import os
import subprocess
import time
import platform

#Tuples (m, n) to test
tests = []

for i in range(1, 20):
    for j in range(i, 20):
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

f.close()