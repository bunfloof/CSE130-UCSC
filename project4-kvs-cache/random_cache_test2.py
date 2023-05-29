'''
KEEP IN MIND THAT THESE ARE STRICTLY STUDENT MADE TESTS. JUST BECAUSE I CREATED THIS TEST, I AM IN NO WAY GUARANTEED TO BE 
CORRECT AND YOU SHOULD ACCOUNT FOR THAT. PASSING THIS TEST DOES NOT GUARANTEE ANY GRADE ON THE ACTUAL ASSIGNMENT,
SO DO NOT COMPLAIN. THESE ARE ONLY HERE FOR YOUR LUXURY.
'''

#IMPORTANT Note: This python script uses functions from the 'request' python library.
#If you do not have this library installed, please do so with 'pip install requests'

import os
import sys
import random
import subprocess
import requests
import json
import time

#Creates a random operation of either SET or GET type, returns as list
def random_operation(cachesize):
    operation_type = "SET" if random.randint(1, 2) == 1 else "GET"

    file_input = str(random.randint(0, 9))

    contents = ""
    if(operation_type == "SET"):
        contents = f"New {file_input}"

    return [operation_type, file_input, contents]
#Example:  [SET, 4, New 4] or [GET, 2, ""]


def run_client(folder, testing_files, policy, cachesize, optotal, dir_size):
    #Keeps track of changed & unchanged files
    #At the start of each test, there are no changed files.
    #A changed file x will have contents "New x", while unchanged will have "Original x" 
    dir_size = 10
    changed_files =[]
    unchanged_files = [str(file) for file in range(dir_size)]

    
    #Generate the input for the program. At the same time, check which files should change and which shouldn't (based on number of SET commands)
    total_input = ""
    for op in optotal:
        if(op[0] == "SET" and op[1] in unchanged_files): 
            changed_files.append(op[1])
            unchanged_files.remove(op[1])

        #Generate input string
        total_input += ' '.join(op).strip(' ')
        total_input += '\n'

    #write to input.txt for later reference
    os.system(f"echo \"{total_input}\" > {folder}/input.txt")

   
    # Create/Reset Files 0 - 9 with contents.
    for x in range(dir_size):
        os.system(f"echo \"Original {str(x)}\" > {folder}/{testing_files}/{str(x)}")

    #Run the program with valgrind. Pipe input into program. Redirect output to output.txt
    process = subprocess.Popen(f"echo \"{total_input}\" | valgrind --leak-check=full --log-file={folder}/valgrind_dump.txt ./client {folder}/{testing_files} {policy} {cachesize} > {folder}/output.txt", shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)

    stdout, stderr = process.communicate()

    #Record the return code
    return_value = process.returncode

    if(return_value != 0):
        #This is a critical error and causes a termination. Means you're accessing invalid memory or some other error occurred.
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


    passed_trial = True
    '''
    The file check works ALMOST all of the time. For example, it's gone until Trial 2000+ before giving an error with the files.
    The error is entirely a python issue as far as I can tell, as ./client did actually generate the right directory.
    If something like this happens to you, put print statements within this script to determine what is causing
    passed_trial to be set to False. If you believe your code is correct, simply copy paste the
    program parameters (input.txt, cache size, policy) into the discord and check with others on what the output should be.
    '''

    #Comment this out if you don't want to check Final Directory State
    ##################################################################
    
    #use grep to find the contents of the changed/unchanged files.
    for file in changed_files:
        grep_find_new = os.system(f"grep \"New {file}\" {folder}/{testing_files}/{file} -q")
        if grep_find_new != 0:  #grep did not find the text within the file, so failed trial.
            passed_trial = False
    
    for file in unchanged_files:
        grep_find_original = os.system(f"grep \"Original {file}\" {folder}/{testing_files}/{file} -q")
        if grep_find_original != 0:  #grep did not find the text within the file, so failed trial.
            passed_trial = False
        
    ##################################################################





    #This section tests if GET/SET stats match

    #Send request with current parameters to server to perform calculations.
    response = requests.post("http://100.21.194.47:5000/calculate", data=json.dumps({"policy": policy, "cache_size": cachesize, "optotal": optotal}))
    stats = json.loads(response.text)

    #These are the expected numbers for the stats. These calculations are performed by me on a server backend to keep it hidden.
    #Keep in mind that this means that if I calculated it wrong, they could be wrong. So please bring up any concerns in the discord by sharing the specific input and comparing outputs.
    #As long as most students are matching a certain output, that's all we can really work towards.
    get_cache = stats[0]
    get_disk = stats[1]
    set_cache = stats[2]
    set_disk = stats[3]

    #To test if stats match, grep the specified string with -q flag to shut up.
    stat_grep_return = 0
    stat_grep_return += os.system(f"grep \"GET COUNT (CACHE): {get_cache}\" {folder}/output.txt -q")
    stat_grep_return += os.system(f"grep \"GET COUNT (DISK): {get_disk}\" {folder}/output.txt -q")
    stat_grep_return += os.system(f"grep \"SET COUNT (CACHE): {set_cache}\" {folder}/output.txt -q")
    stat_grep_return += os.system(f"grep \"SET COUNT (DISK): {set_disk}\" {folder}/output.txt -q")

    #If any greps return non-zero, this means failure.

    #Comment this out if you don't want to check statistics
    ########################################################
    if stat_grep_return > 0:
        passed_trial = False
    ########################################################

    #Procedure if you did not pass the trial. Print the details, input, & difference in outputs.
    if not passed_trial:
        print(f"Incorrect result on Trial {n}. Stats for Trial {n} are shown below")

        print("======================================================\n")
        print(f"Trial: {n}, Cache Size: {cachesize}, Policy: {policy}")
        print(f"This is what was inputted to your program:\n{total_input}")
        print("======================================================\n")

        print("Expected files after execution:")

        #print out expected files + contents
        for file in range(dir_size):
            if(str(file) in unchanged_files):
                print(f"{str(file)}: Original {str(file)}")
            else:
                print(f"{str(file)}: New {str(file)}")
                
        #print out what YOUR directory actually is
        print()
        print("Your files after execution:")
        for file in range(dir_size):
            with open(f"{folder}/{testing_files}/{str(file)}") as f:
                print(f"{str(file)}: {f.read().strip()}")    
        
        print()
        print()

        getpercent = "%nan" #Yes, I understand that this may not be the exact syntax that the program should be outputting.
        setpercent = "%nan" #However, do note that the % values are not compared when determining failure, only the GET/SET numbers are.
        
        #Math is lame
        if get_cache != 0:            
            getpercent = round(100 * (get_cache - get_disk)/get_cache, 2)
        
        if set_cache != 0:            
            setpercent = round(100 * (set_cache - set_disk)/set_cache, 2)
        
        #if grep didnt return SUCCESS every time, there was a discrepency
        if stat_grep_return > 0:
            print(f"Expected end-of-program statistics:")
            print(f"GET COUNT (CACHE): {get_cache}")
            print(f"GET COUNT (DISK): {get_disk}")
            print(f"GET CACHE HIT RATE: {getpercent}%")
            print(f"SET COUNT (CACHE): {set_cache}")
            print(f"SET COUNT (DISK): {set_disk}")
            print(f"SET CACHE HIT RATE: {setpercent}%")
        
            print()
            print()
            #Your stats (I print out only the lines with a ':', to weed out the outputs of GET commands)
            print(f"Your end-of-program statistics:")
            with open(f"{folder}/output.txt") as f:
                for i in f.readlines():
                    if i.find(':') > -1:
                        print(i.strip())




    #Regardless of correct output or not, check valgrind_dump.txt for memory leaks

    #Comment this part out if you don't want to check for memory leaks
    ##################################################################

    with open(f"{folder}/valgrind_dump.txt") as f:

        #use grep to locate "All heap blocks freed" statement within the valgrind dump file. If not found, error.
        grep_valgrind = os.system(f"grep \"All heap blocks were freed -- no leaks are possible\" {folder}/valgrind_dump.txt -q")
        if grep_valgrind != 0:
            passed_trial = False
            print(f"Valgrind detected memory leaks in Trial {n}. Check {folder}/valgrind_dump.txt for more details.")

            print("Stats for the program call are shown below")
            print("======================================================\n")
            print(f"Trial: {n}, Cache Size: {cachesize}, Policy: {policy}")
            print(f"This is what will be inputted to your program:\n{total_input}")
            print("======================================================\n")

    ##################################################################


    return passed_trial
    





#PROGRAM INITIALIZATION

#Names of the directories to be referenced throughout the program. You may change these if you like.
folder = "rcache_test_dir"          #This is the folder everything will be put into, including input.txt, output.txt, & valgrind_dump.txt
testing_files = "testing_files"     #This is a folder within the testing folder. It contains the files that YOUR program will run on.


#This segment clears out the testing_files folder for safety.
if os.path.exists(f"{folder}/{testing_files}"):
    os.system(f"rm {folder}/{testing_files}/*")

#This segment creates the necessary folders if they are not present already.
if not os.path.exists(folder):
    print(f"{folder} not found. Creating...")
    os.system(f"mkdir {folder}")
    os.system(f"mkdir {folder}/{testing_files}")

#Create/reset the valgrind dump file.
os.system(f"echo > {folder}/valgrind_dump.txt")

#Create 10 files with contents "Original <number>"
for x in range(10):
        os.system(f"echo \"Original {str(x)}\" > {folder}/{testing_files}/{str(x)}")

#Valid policies are originally all. To be specified by user.
valid_policies = ["FIFO", "CLOCK", "LRU"]

if(not (len(sys.argv) == 3)):
    print(f"INCORRECT USAGE: Call with  \"{sys.argv[0]} [FIFO|CLOCK|LRU|ALL] NUM_TRIALS\"")
    exit()

dir_size = 10


#Command line argument check
policy = sys.argv[1]
TRIALS = int(sys.argv[2])

if(TRIALS < 0):
    print("ERROR: Enter a valid integer for NUM_TRIALS.")
    exit()
if(policy != "ALL"):    
    if(policy not in valid_policies):
        print(f"INVALID POLICY: Call with  \"{sys.argv[0]} [FIFO|CLOCK|LRU|ALL] NUM_TRIALS\"")
        # print("(DIR_SIZE is the number of files to work on. Default is 10)")
        exit()
    else:
        valid_policies = [policy]

print(f"Running {TRIALS} trials with the following policies:")
for i in valid_policies:
    print(i, end=" ")
print()
print()

start = time.time()

#Loop through each trial
for n in range(1, TRIALS + 1):

    #Generate program parameters

    #Cache size is random between 1 and 5. (This does not test for cache size = 0)
    cachesize = random.randint(1, 5)

    #Generate command sequence. Length = 2 * cache size
    optotal = []
    for _ in range(1, cachesize* 2 + 1):
        optotal.append(random_operation(cachesize))

    #Run run_client for every policy specified by user
    for policy in valid_policies:
        passed = run_client(folder, testing_files, policy, cachesize, optotal, dir_size)
        if passed:
            print(f"Trial {n}, Policy {policy} passed.")
        else:
            exit()
    #Record time
    elapsed = time.time() - start
    print(f"Total time elapsed: {round(elapsed/60, 2)} minutes")
    print()

print(f"All {TRIALS} trials passed in {round(elapsed/60, 2)} minutes. Congratulations.")


#Here's a treat if you made it this far
elapsed = time.time() - start
if(TRIALS >= 1000 and len(valid_policies) == 3 and elapsed/60 < 45):
    print("You've obtained the secret ending! Here's something for the rest of your travels :)")
    print("\n⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠛⠛⠛⠋⠉⠈⠉⠉⠉⠉⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿\n⣿⣿⣿⣿⡏⣀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿\n⣿⣿⣿⢏⣴⣿⣷⠀⠀⠀⠀⠀⢾⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿\n⣿⣿⣟⣾⣿⡟⠁⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣷⢢⠀⠀⠀⠀⠀⠀⠀⢸⣿\n⣿⣿⣿⣿⣟⠀⡴⠄⠀⠀⠀⠀⠀⠀⠙⠻⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⣿\n⣿⣿⣿⠟⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⢴⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⣿\n⣿⣁⡀⠀⠀⢰⢠⣦⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⣿⡄⠀⣴⣶⣿⡄⣿\n⣿⡋⠀⠀⠀⠎⢸⣿⡆⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⠗⢘⣿⣟⠛⠿⣼\n⣿⣿⠋⢀⡌⢰⣿⡿⢿⡀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣿⣿⡇⠀⢸⣿⣿⣧⢀⣼\n⣿⣿⣷⢻⠄⠘⠛⠋⠛⠃⠀⠀⠀⠀⠀⢿⣧⠈⠉⠙⠛⠋⠀⠀⠀⣿⣿⣿⣿⣿\n⣿⣿⣧⠀⠈⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠟⠀⠀⠀⠀⢀⢃⠀⠀⢸⣿⣿⣿⣿\n⣿⣿⡿⠀⠴⢗⣠⣤⣴⡶⠶⠖⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡸⠀⣿⣿⣿⣿\n⣿⣿⣿⡀⢠⣾⣿⠏⠀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠉⠀⣿⣿⣿⣿\n⣿⣿⣿⣧⠈⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿\n⣿⣿⣿⣿⡄⠈⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⣦⣄⣀⣀⣀⣀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠙⣿⣿⡟⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠁⠀⠀⠹⣿⠃⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢐⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⠿⠛⠉⠉⠁⠀⢻⣿⡇⠀⠀⠀⠀⠀⠀⢀⠈⣿⣿⡿⠉⠛⠛⠛⠉⠉\n⣿⡿⠋⠁⠀⠀⢀⣀⣠⡴⣸⣿⣇⡄⠀⠀⠀⠀⢀⡿⠄⠙⠛⠀⣀⣠⣤⣤⠄⠀")
