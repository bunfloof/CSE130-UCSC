# icockie

import os
import sys
import random
import subprocess

#Creates a random operation of either SET or GET type, returns as list
def random_operation(cachesize):
    operation_type = "SET" if random.randint(1, 2) == 1 else "GET"

    file_input = str(random.randint(0, 9))

    contents = ""
    if(operation_type == "SET"):
        contents = f"New {file_input}"

    return [operation_type, file_input, contents, "\n"]


def run_client(folder, testing_files, policy, cachesize, optotal, dir_size):
    #Keeps track of changed & unchanged files
    dir_size = 10
    changed_files =[]
    unchanged_files = [str(file) for file in range(dir_size)]

    
    #Generate the input for the program
    total_input = ""
    for op in optotal:
        if(op[0] == "SET" and op[1] in unchanged_files): 
            changed_files.append(op[1])
            unchanged_files.remove(op[1])

        #Generate input string
        for i in op:
            total_input += i + " " * (i != '\n')



   
    # Create/Reset Files 1 - 10 with contents.
    for x in range(dir_size):
        os.system(f"echo \"Original {str(x)}\" > {folder}/{testing_files}/{str(x)}")

    # print(f"\"{total_input}\"")

    # return_value = os.system(f"valgrind --leak-check=full --log-file={folder}/valgrind_dump.txt ./client {folder}/{testing_files} {policy} {cachesize} > {folder}/output.txt")

    # valgrind_args = [
    #     "valgrind",
    #     "--leak-check=full",
    #     f"--log-file={folder}/valgrind_dump.txt",
    #     "./client",
    #     f"{folder}/{testing_files}",
    #     f"{policy}",
    #     f"{cachesize}"
    # ]


    #KNOWN ERRORS: For whatever reason, despite redirecting stdout to a file, this doesn't capture any intermediate outputs by ./client
    #This includes the output of GET operations. However, it still prints the end of program stats and redirects that to output.txt
    process = subprocess.Popen(f"echo \"{total_input}\" | valgrind --leak-check=full --log-file={folder}/valgrind_dump.txt ./client {folder}/{testing_files} {policy} {cachesize} > {folder}/output.txt", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)

    stdout, stderr = process.communicate()

    return_value = process.returncode

    if(return_value != 0):
        print(f"Ran into return errors on Trial {n}:")
        print(f"valgrind --leak-check=full --log-file={folder}/valgrind_dump.txt ./client {folder}/{testing_files} {policy} {cachesize} > {folder}/output.txt")
        print(f"Returned {return_value}")
        print(f"This likely means valgrind ran into an invalid memory access error. Check {folder}/valgrind_dump.txt for details")
        
        print()
        print("Here are the stats for the program call")
        print("======================================================\n")
        print(f"Trial: {n}, Cache Size: {cachesize}, Policy: {policy}")
        print(f"This is what was inputted to your program:\n{total_input}")
        print("======================================================\n")

        exit()


    #Test whether directory contents are correct
    passed_trial = True
    for file in changed_files:
        with open(f"{folder}/{testing_files}/{file}") as f:
            if "New" not in f.read():
                passed_trial = False
    
    for file in unchanged_files:
        with open(f"{folder}/{testing_files}/{file}") as f:
            if "Original" not in f.read():
                passed_trial = False
    

    if not passed_trial:
        print(f"Incorrect result on Trial {n}. Stats for Trial {n} are shown below")

        print("======================================================\n")
        print(f"Trial: {n}, Cache Size: {cachesize}, Policy: {policy}")
        print(f"This is what was inputted to your program:\n{total_input}")
        print("======================================================\n")

        print("Expected files after execution:")

        for file in range(dir_size):
            if(str(file) in unchanged_files):
                print(f"{str(file)}: Original {str(file)}")
            else:
                print(f"{str(file)}: New {str(file)}")
                

        print()
        print("Your files after execution:")
        for file in range(dir_size):
            with open(f"{folder}/{testing_files}/{str(file)}") as f:
                print(f"{str(file)}: {f.read().strip()}")    

    #Regardless of correct output or not, check valgrind_dump.txt for memory leaks
    with open(f"{folder}/valgrind_dump.txt") as f:
        l = f.readlines()
        if "All heap blocks were freed -- no leaks are possible" not in l[11]:
            passed_trial = False
            print(f"Valgrind detected memory leaks in Trial {n}. Check {folder}/valgrind_dump.txt for more details.")

            print("Stats for the program call are shown below")
            print("======================================================\n")
            print(f"Trial: {n}, Cache Size: {cachesize}, Policy: {policy}")
            print(f"This is what will be inputted to your program:\n{total_input}")
            print("======================================================\n")

    return passed_trial
    



folder = "cache_test_dir"
testing_files = "testing_files"


if os.path.exists(f"{folder}/{testing_files}"):
    os.system(f"rm {folder}/{testing_files}/*")

if not os.path.exists(folder):
    print(f"{folder} not found. Creating...")
    os.system(f"mkdir {folder}")
    os.system(f"mkdir {folder}/{testing_files}")

os.system(f"echo > {folder}/valgrind_dump.txt")


for x in range(10):
        os.system(f"echo \"Original {str(x)}\" > {folder}/{testing_files}/{str(x)}")


valid_policies = ["FIFO", "CLOCK", "LRU"]

if(not (len(sys.argv) == 3 or len(sys.argv) == 4)):
    print(f"INCORRECT USAGE: Call with  \"{sys.argv[0]} [FIFO|CLOCK|LRU|ALL] NUM_TRIALS\"")
    # print("(DIR_SIZE is the number of files to work on. Default is 10)")
    exit()

dir_size = 10
if(len(sys.argv) == 4):
    dir_size = int(sys.argv[3])
    if(dir_size <= 0):
        print("ERROR: Enter a non-zero integer for DIR_SIZE.")
        exit()

case = sys.argv[1]
TRIALS = int(sys.argv[2])

if(TRIALS < 0):
    print("ERROR: Enter a valid integer for NUM_TRIALS.")
    exit()
if(case != "ALL"):    
    if(case not in valid_policies):
        print(f"INVALID POLICY: Call with  \"{sys.argv[0]} [FIFO|CLOCK|LRU|ALL] NUM_TRIALS\"")
        # print("(DIR_SIZE is the number of files to work on. Default is 10)")
        exit()
    else:
        valid_policies = [case]


print(f"Running {TRIALS} trials with the following policies:")
for i in valid_policies:
    print(i, end=" ")
print()
print()

for n in range(1, TRIALS + 1):

    #Generate program parameters
    cachesize = random.randint(1, 5)

    optotal = []
    for _ in range(1, cachesize* 2 + 1):
        optotal.append(random_operation(cachesize))


    for case in valid_policies:
        passed = run_client(folder, testing_files, case, cachesize, optotal, dir_size)
        if passed:
            print(f"Trial {n}, Policy {case} passed.")
        else:
            exit()
    print()

print("All trials passed. Congratulations")

if(TRIALS >= 100 and len(valid_policies) == 3):
    print("You've obtained the secret ending! Here's something for the rest of your travels :)")
    print("\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡴⠚⣉⡙⠲⠦⠤⠤⣤⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⢀⣴⠛⠉⠉⠀⣾⣷⣿⡆⠀⠀⠀⠐⠛⠿⢟⡲⢦⡀⠀⠀⠀⠀\n⠀⠀⠀⠀⣠⢞⣭⠎⠀⠀⠀⠀⠘⠛⠛⠀⠀⢀⡀⠀⠀⠀⠀⠈⠓⠿⣄⠀⠀⠀\n⠀⠀⠀⡜⣱⠋⠀⠀⣠⣤⢄⠀⠀⠀⠀⠀⠀⣿⡟⣆⠀⠀⠀⠀⠀⠀⠻⢷⡄⠀\n⠀⢀⣜⠜⠁⠀⠀⠀⢿⣿⣷⣵⠀⠀⠀⠀⠀⠿⠿⠿⠀⠀⣴⣶⣦⡀⠀⠰⣹⡆\n⢀⡞⠆⠀⣀⡀⠀⠀⠘⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢿⣿⣶⠇⠀⢠⢻⡇\n⢸⠃⠘⣾⣏⡇⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⣠⣤⣤⡉⠁⠀⠀⠈⠫⣧\n⡸⡄⠀⠘⠟⠀⠀⠀⠀⠀⠀⣰⣿⣟⢧⠀⠀⠀⠀⠰⡿⣿⣿⢿⠀⠀⣰⣷⢡⢸\n⣿⡇⠀⠀⠀⣰⣿⡻⡆⠀⠀⠻⣿⣿⣟⠀⠀⠀⠀⠀⠉⠉⠉⠀⠀⠘⢿⡿⣸⡞\n⠹⣽⣤⣤⣤⣹⣿⡿⠇⠀⠀⠀⠀⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡔⣽⠀\n⠀⠙⢻⡙⠟⣹⠟⢷⣶⣄⢀⣴⣶⣄⠀⠀⠀⠀⠀⢀⣤⡦⣄⠀⠀⢠⣾⢸⠏⠀\n⠀⠀⠘⠀⠀⠀⠀⠀⠈⢷⢼⣿⡿⡽⠀⠀⠀⠀⠀⠸⣿⣿⣾⠀⣼⡿⣣⠟⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⢠⡾⣆⠑⠋⠀⢀⣀⠀⠀⠀⠀⠈⠈⢁⣴⢫⡿⠁⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⣧⣄⡄⠴⣿⣶⣿⢀⣤⠶⣞⣋⣩⣵⠏⠀⠀⠀⠀⠀\n⠀⠀⠀⠀⠀⠀⠀⠀⠀⢺⣿⢯⣭⣭⣯⣯⣥⡵⠿⠟⠛⠉⠉⠀⠀⠀⠀⠀⠀⠀")