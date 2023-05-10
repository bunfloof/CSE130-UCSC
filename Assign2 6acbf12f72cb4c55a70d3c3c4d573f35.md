# Assign2

published: No

# Assignment 2

This assignment assesses your understanding of the materials covered in Chapter 2.  You can submit responses as many times as you like before the deadline. We will grade the last submission.

## Question at position 1

Describe what a process and thread are. What is the difference between them? How should one decide to use a process or thread?

A process is an abstraction of a running program that consists of an instance of an executing program, including the current value of the program counter, registers, and variables. Conceptually each process has its own virtual CPU. Processes allow for (pseudo) concurrent operation, enabling a single CPU to perform multiple tasks by rapidly switching between processes, creating an illusion of parallelism.

A thread is a lightweight, smaller unit of execution within a process. Threads have multiple threads of control within the same address space running in quasi-parallel. They share the same address space and resources of the parent process. This enable parallel entities to share data among themselves, making them easier and faster to create and destroy.

The differences between processes and threads are:

1. Processes have their own address space, while threads share the same address space within the parent process.
2. Processes are more resource-intensive than threads, which are lighter weight and faster to create and destroy.
3. Inter-process communication is more complicated than inter-thread communication due to the shared address space among threads.

To decide whether one should use a process a thread, consider:

1. Resource requirements: if an application require a large amount of resources or needs isolation form other executing programs, processes are more suitable. If the application requires less overhead and can share resources with other activities, threads are more suitable.
2. Concurrency: threads allow for better concurrency within a shared address space, making them suitable for applications that require multiple parallel activities to access shared data. Processes are more suitable for independent activities that do not need to share information.
3. Ease of communication: Threads communicate more easily due to the shared address space. If the application requires frequent and simple communication between parallel activities, threads might be more suitable because processes require more complex inter-process communication mechanisms.

## Question at position 2

Describe what a race condition is.

A race condition is an occurrence when two or more processes access shared data concurrently, and the final result depends on the relative timing of their execution. In a race condition, the behavior of the system is unpredictable and can lead to unexpected and undesirable outcomes. 

## Question at position 3

What is process synchronization?

Process synchronization is the coordination of activities and management of shared resources among multiple concurrent processes or threads. It ensures that multiple processes or threads can execute concurrently without causing conflicts or inconsistencies when accessing shared resources, such as memory, files, or devices. Synchronization mechanisms enforce the correct order of execution to prevent issues like race conditions or deadlocks. The synchronization techniques include using semaphores, mutexes, monitors, and message passing to control access to shared resources and coordinate the execution of concurrent processes or threads.

## Question at position 4

Describe each of the following scheduling algorithms: “FCFS” “Round-Robin Scheduling” “Shortest Job First” “Priority Scheduling”

First-Come, First-Served (FCFS):
FCFS is a non-preemptive scheduling algorithm that assigns the CPU to processes in the order they request it. The ready processes are placed in a single queue, and when a running process blocks, the first process in the queue is run next. FCFS is simple and easy to implement but has the disadvantage of not prioritizing short tasks over long tasks, which can lead to inefficient CPU usage.

Round-Robin Scheduling:
Round-robin scheduling is a preemptive algorithm that assigns each process a time interval called a quantum. If a process is still running at the end of its quantum, the CPU is preempted and given to another process. The algorithm maintains a list of runnable processes and moves a process to the end of the list when its quantum is used up. Round-robin scheduling offers a fair approach to distributing CPU time, but the choice of quantum length is crucial for balancing efficiency and responsiveness.

Shortest Job First (SJF):
SJF is a non-preemptive batch scheduling algorithm that selects the process with the shortest runtime from the input queue. This algorithm is optimal for minimizing the average turnaround time when all jobs are available simultaneously. However, it requires knowing the runtime of each process in advance.

Priority Scheduling:
Priority scheduling is an algorithm that assigns a priority to each process, with the runnable process with the highest priority being allowed to run. Priorities can be assigned statically (e.g., based on the user) or dynamically (e.g., to favor I/O-bound processes). To prevent high-priority processes from running indefinitely, the scheduler may decrease the priority of the running process at each clock tick or assign a maximum time quantum. This algorithm allows for more customized resource allocation but may require more complex priority assignment mechanisms.

## Question at position 5

The roles and initial values of each semaphore are:

1. Empty slots (emptySemaphore): This semaphore represents the number of empty slots available in the buffer for the producer to insert items. The initial value of emptySemaphore is set to N, the maximum capacity of the buffer.
2. Filled slots (filledSemaphore): This semaphore represents the number of filled slots in the buffer containing items ready for the consumer to consume. The initial value of filledSemaphore is set to 0, as there are no items in the buffer initially.
3. Mutex (mutexSemaphore): This semaphore is used to ensure mutual exclusion while accessing the shared buffer. This prevents the producer and consumer from simultaneously modifying the buffer, avoiding race conditions. The initial value of mutexSemaphore is set to 1, allowing one thread to access the buffer at a time.

I would use three semaphores with the initial values:

- emptySemaphore: N (number of empty slots)
- filledSemaphore: 0 (number of filled slots)
- mutexSemaphore: 1 (to ensure mutual exclusion)

The producer and consumer would perform the actions:

Producer:

1. Wait on emptySemaphore (decreases value, representing fewer empty slots)
2. Wait on mutexSemaphore (ensuring exclusive access to buffer)
3. Insert item into buffer
4. Signal mutexSemaphore (releasing exclusive access to buffer)
5. Signal filledSemaphore (increasing value, representing more filled slots)

Consumer:

1. Wait on filledSemaphore (decreases value, representing fewer filled slots)
2. Wait on mutexSemaphore (ensuring exclusive access to buffer)
3. Remove item from buffer
4. Signal mutexSemaphore (releasing exclusive access to buffer)
5. Signal emptySemaphore (increasing value, representing more empty slots)