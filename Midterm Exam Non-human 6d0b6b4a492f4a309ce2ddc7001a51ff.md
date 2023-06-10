# Midterm Exam Non-human

published: No

### True/False Questions

- **False** The operating system is not responsible for resource allocation between competing processes
- **True** An interrupt table contains the addresses of the handlers for the various interrupts.
- **True** DMA (Direct Memory Access) is a mechanism for allowing an I/O device to transfer data to and from memory without involving the CPU in the transfer.
- **False** Each thread of a process has its own virtual address space.
- **True** An advantage of implementing threads in user space is that they don't incur the overhead of having the OS schedule their execution.
- **False** When the CPU is in kernel mode, the CPU is prevented from accessing areas of memory that are not owned by the current process.
- **False** In round robin scheduling, it is advantageous to give each I/O bound process a longer quantum than each CPU-bound process.
- **False** If N batch jobs are scheduled using Shortest Job First scheduling, the total time it takes to execute the N processes is less than using any other batch schedule (assuming no two jobs take the same time).
- **False** If a system has a small turn around time (the time between submission and termination), then it usually means that this system has a large throughput.
- **True** Deadlock can never occur if no process is allowed to hold a resource while requesting another resource.
    
    

### Multiple-choice questions

1. There four conditions are necessary for a resource deadlock to occur. Select the one is NOT belonged to the four conditions.
    
    (a) Mutual exclusion 
    
    (b) Hold and wait 
    
    **(c) Preemption** 
    
    (d) Circular wait
    

**Answer: Preemption** 

1. Which of these statements is true?
    
    **(a) A mode switch precedes a context switch.** 
    
    (b) A context switch precedes a mode switch. 
    
    (c) A context switch can occur without a mode switch. 
    
    (d) A mode switch is just a different name for a context switch.
    

**Answer: A mode switch precedes a context switch.** 

1. A computer has 10 tape drives, with n processes competing for them. Each process may need up to two drives. For which maximum value of n is the system guaranteed to be deadlock free?
    
    (a) 1
    **(b) 9**
    (c) 10
    (d) 20
    

**Answer: 9**

1. What needs to be saved and restored on a context switch between two threads in the same process? We need to save the _____ into the TCB of the thread that is no longer running
    
    (a) processor registers.
    (b) stack pointer
    (c) program counter
    **(d) all of the mentioned**
    

**Answer: all of the mentioned**

1. Which of the following is true of a blocking system call?
    
    (a) The application continues to execute its code when the call is issued. 
    (b) The call returns immediately without waiting for the I/O to complete. 
    **(c) The execution of the application is suspended when the call is issued.** 
    (d) Blocking application code is harder to understand than nonblocking application code
    

**Answer: The execution of the application is suspended when the call is issued.** 

1. A computer system has enough room to hold 5 programs in its main memory. These programs are idle waiting for I/O half the time. What fraction of the CPU time is wasted (idle)?
    
    (a) 1/5 
    (b) 1/10 
    (c) 31/32 
    **(d) 1/32**
    

**Answer: 1/32**

1. How many philosophers may eat simultaneously in the Dining Philosophers problem with 7 philosophers?
    
    (a) 1
    (b) 2
    **(c) 3**
    (d) 5
    
    **Answer: 3**
    
2. Given the graph as follows,
    
    ![Given the graph as follows.svg](Midterm%20Exam%20Non-human%206d0b6b4a492f4a309ce2ddc7001a51ff/Given_the_graph_as_follows.svg)
    
    Assume a process P0 needs 5 cycles to terminate and requests an I/O of 3 CPU cycles. And there is another process Pl in the same ready queue, and it only requires 5 CPU cycles to terminate. If we can see this timeline for P0 and Pl, then the OS is using ___ scheduling algorithm.
    
    (a) First In First Service
    (b) Short Job First
    (c) Round Robin
    **(d) Priority-based Scheduling**
    
    **************Answer: Priority-based Scheduling**************
    
    | Time  | P_0_status | P_1_status: |
    | --- | --- | --- |
    | 1 | Running | Ready |
    | 2 | Running | Ready |
    | 3 | Running | Ready |
    | 4 | Blocked | Running |
    | 5 | Blocked | Running |
    | 6 | Blocked | Running |
    | 7 | Ready | Running |
    | 8 | Running | Blocked |
    | 9 | Running | Blocked |
    | 10 | - | Running |
3. Which of the following is most likely to be a system call?
(a) The implementation of a while loop in C.
(b) Parse a token from a string.
(c) Get the cosine of a number
**(d) Get the time of day.**
    
    **Answer: Get the time of day.**
    
4. Regarding the difference between preemptive and non-preemptive scheduling, which one is true?
    
    (a) Preemptive scheduling allows a process to be interrupted in the midst of its execution, taking the CPU away and allocating it to another ready process.
    (b) Non-preemptive scheduling ensures that a process relinquishes control of the CPU only when it finishes with its current CPU burst..
    **(c) All of the mentioned (A and B).**
    (d) None of the mentioned (A and B).
    
    **Answer: All of the mentioned (A and B).**
    

### Short answer questions

1. Name three ways in which the processor can transition from user mode to kernel mode?
- The user process can execute a trap instruction (e.g. system call). A trap is known as a synchronous software interrupt.
- The user process can cause an exception (divide by zero, access bad address, bad instruction, page fault, etc).
- The processor can transition into kernel mode when receiving an I/O interrupt.
1. What is the difference between deadlock and starvation? 
- In a deadlock situation, none of the involved processes can possibly make progress.
- In a starvation situation, a process is ready to execute, but it is not being allowed to execute. H-40
1. What is context switching?
- Transferring the control from one process to other process requires saving the state of the old process and loading the saved state for new process. This task is known as context switching.
1. What is mutex?
- A mutex is a **lock**. It is a **program object** that allows multiple program threads to share the same resource, such as file access **but not simultaneously**.
1. What is the Semaphore?
- A semaphore is an integer variable that, apart from initialization, is accessed only through two standard **atomic** operations: **down, and up**.
- **Down checks semaphore**. If not zero, decrements semaphore. If zero, process goes to sleep
- **Up increments semaphore**. If more than one process asleep, one is chosen
1. Most operating systems are designed for general-purpose computation. A proposal has been put forth for an OS that is optimized for running math intensive programs. In MathOS, the kernel includes system calls for many useful mathematical operations, such as matrix arithmetic, Bessel functions, Euclidean distance, etc. These system calls are written in highly optimized assembly language for maximum performance. Is this concept for MathOS a good idea? Explain why or why not.
    
    No! 
    
    Key part: Math functions won't benefit from running in the kernel. They do not need privileged instructions or special facilities. You're putting crap in the kernel that has no reason to be there. 
    
    Moreover: The overhead of calling them: mode switch + data copy is more time- consuming than a function call 
    
    NOT: Assembly code is difficult to write/read/debug/etc.
    
2. At the beginning of the COVID lockdown, I downloaded a 100MB large directory of high-resolution Zoom background images from the university website and added it to the Zoom client on my computer. Now, whenever I open the Settings tab in Zoom and try to go to the virtual background tab (where a preview of all the images will be shown), my Zoom client appears to stall. The User Interface (UI) becomes unresponsive for almost half a minute before the images finally appear in the UI. This very annoying situation me thinking about process states.
    
    1. While the Zoom client is unresponsive, what process state is it most likely in (RUNNING, READY, or BLOCKED)? Justify your hypothesis.
    
    2. How could I confirm or reject my hypothesis? 
    
    [Solution] It's most likely in the RUNNING state (presumably busy running image processing code to create thumbnails on the fly for the large images).
    
    I could verify this by observing my system's CPU utilization, which should hover around 100% if the process was in the RUNNING state. This is, in fact, what happened and it also caused the CPU's fan to kick in, making noticeable noise.
    
    If the CPU utilization had not been at 100%, the process would have been in the BLOCKED state. This could occur, hypothetically, if the program tried to repeatedly contact a remote server with very slow response times.
    
3. Here is a table of processes and their associated arrival and running times.
    
    
    | Process ID | Arrival Time | CPU Running Time |
    | --- | --- | --- |
    | Process 1 | 0 | 5 |
    | Process 2 | 4 | 5 |
    | Process 3 | 2 | 1 |
    | Process 4 | 7 | 2 |
    | Process 5 | 8 | 3 |
    
    Show the scheduling order for these processes under 3 policies: First Come First Serve (FCFS), Shortest-Job-First (SJF), Round-Robin (RR) with time slice quantum = 1.
    
    *Assume that context switch overhead is 0 and that new RR processes are added to the **head** of the queue and new FCFS processes are added to the **tail** of the queue.*
    
    | Time slots | FCFS | SJF (non-preemption) | SJF (Preemption) | RR |
    | --- | --- | --- | --- | --- |
    | 0 | P1 | P1 | P1 | P1 |
    | 1 | P1 | P1 | P1 | P1 |
    | 2 | P1 | P1 | P3 | P3 |
    | 3 | P1 | P1 | P1 | P1 |
    | 4 | P1 | P1 | P1 | P2 |
    | 5 | P3 | P3 | P1 | P1 |
    | 6 | P2 | P2 | P2 | P2 |
    | 7 | P2 | P2 | P4 | P4 |
    | 8 | P2 | P2 | P4 | P5 |
    | 9 | P2 | P2 | P5 | P1 |
    | 10 | P2 | P2 | P5 | P2 |
    | 11 | P4 | P4 | P5 | P4 |
    | 12 | P4 | P4 | P2 | P5 |
    | 13 | P5 | P5 | P2 | P2 |
    | 14 | P5 | P5 | P2 | P5 |
    | 15 | P5 | P5 | P2 | P2 |
    
    For each process in each schedule above, indicate the queue wait time. Note that wait time is the total time spend waiting in queue (all the time in which the task is not running)
    
    | Scheduler | Process 1 | Process 2 | Process 3 | Process 4 | Process 5 |
    | --- | --- | --- | --- | --- | --- |
    | FCFS wait | 0 | 2 | 3 | 4 | 5 |
    | SJF (non-preemption) | 0 | 2 | 3 | 4 | 5 |
    | SJF wait (preemption) | 1 | 7 | 0 | 0 | 1 |
    | RR wait | 5 | 7 | 0 | 3 | 4 |
    
    7. Multithreaded Sum
    
    Complete the “`sum_multithreaded`” function that calculates the sum of the integer array using threads. 
    
    The function has the following signature: 
    
    `int sum_multithreaded(int len, int* arr, int num_threads)`
    
    - `len` is the length of the array
    - `arr` is the array of integers of size `len`
    - `num_threads` is the number of threads to create
    - For simplicity, assume that `len` is evenly divisible by `num_threads`.
    
    You are given a helper function `thread_fn` that computes the partial sum of `chunk_size` elements from `start_index` in the array. **Complete the sum_multithreaded function.** You may use the C library functions. Below is a list of library functions and their signatures that you may find useful:
    
    ```bash
    int pthread_create(pthread_t *restrict thread, const pthread_attr_t *restrict attr, void *(*start_routine)(void *), void *restrict arg);
    int pthread_join(pthread_t thread, void **retval);
    void *malloc(size_t size);
    void *calloc(size_t nmemb, size_t size);
    ```
    
    The code below illustrates how sum_multithreaded will be used in the code.
    
    ```bash
    int main(void) {
    	int arr [ ] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    	int result = sum_multithreaded(10, arr, 5);
    	printf("%d\n", result); // -> prints 55
    	return 0;
    }
    	
    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    typedef struct thread_args {
    	int start_index;
    	int chunk_size;
    	int* arr;
    	int partial_sum;
    } thread_args;
    
    void* thread_fn(void* ptr) {
    	// cast the pointer to the correct type
    	thread_args* args. = (thread_args*)ptr;
    	// initialize partial sum to zero
    	args->partial_sum = 0;
    	// calculate sum of `chunk size` elements from index `start`
    	for (int i = 0; i < args->chunk_size; i++) {
    		args->partial_sum += args->arr[args->start_index + i];
    	}
    	return NULL;
    }
    
    int sum__multithreaded (int len, int* arr, int num_threads) {
    	// COMPLETE THIS FUNCTION
    	thread args* args arr = (thread args*)calloc(num threads, sizeof (thread args)); 
    	pthread t* threads = (pthread t*)calloc(num_threads, sizeof(pthread_t));
    	int chunk size = les / num threads;
    	for (int 1 = 0; 1 < num threads; 1++) {
    		args_arr[i].start_index = chunk_size * i;
    		args_arr[i].chunk_size = chunk_size;
    		args_arrli].arr = arr;
    		pthread_create(&threads[i], NULL, thread_fn, &args_arr[1]);
    	for (int i = 0; i < num threads; i++) (
    		pthread_join(threads [1], NULL);
    	}
    	int result = 0;
    	for (int i = 0; 1 < num threads; i++)
    		result += args_arr[i].partial_sum;
    	}
    	return result;
    }
    ```