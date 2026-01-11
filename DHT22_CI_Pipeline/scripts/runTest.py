import subprocess
import sys
import os


COMPILER_TIMEOUT  = 30.0 

make_command = ["make", "test-build"]
execute_command = ["make", "test-run"]

current_dir = os.path.dirname(os.path.abspath(__file__))
repo_root =   os.path.dirname(current_dir)
os.chdir(repo_root)

print("Building...")
try:
        ret = subprocess.run(make_command,
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE,
                                timeout=COMPILER_TIMEOUT)
except Exception as e:
    print("ERROR: Compilation failed.", str(e))
    exit(1)

output_err = ret.stderr.decode('utf-8')

# Check to see if the program compiled successfully
if ret.returncode != 0:
    print(output_err)
    print("Compilation failed.")
    exit(1)

print("Executing ...")
try:
        ret = subprocess.run(execute_command,
                                stdout=subprocess.PIPE,
                                timeout=COMPILER_TIMEOUT)
except Exception as e:
    print("Execution failed.", str(e))
    exit(1)

output = ret.stdout.decode('utf-8')
print(output)

# Check to see if the program compiled successfully
if ret.returncode != 0:
    print("Tests failed.")
    exit(1)

print("ALL TEST PASSED")

exit(0)