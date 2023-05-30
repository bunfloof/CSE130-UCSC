The two main functions of an operating system are providing abstractions to user programs and managing the computer's resources.

1. To elaborate, providing abstractions means that the operating system presents a simplified view or a graphic user interface of the computer to users, hiding the complexity of hardware and software interactions. For example, when a user wants to print a document, they simply click "print" in their application and the operating system handles all the details of communicating with the printer.

2. Managing resources means that the operating system allocates and controls access to hardware resources such as memory, CPU time, and input/output devices. For example, CPU scheduling is when the OS prioritizes and allocates CPU time to different processes to ensure they run efficiently and smoothly. 

References
- "operating systems have two main functions: providing abstractions to user programs and managing the computer’s resources” (p. 50).
- "the interaction between user programs and the operating system deals with...creating, writing, reading, and deleting files” (p. 50).
- "computers are equipped with a layer of software called the operating system...to handle managing all [the computer's] resources” (p. 1).

Tanenbaum. (2015). Modern operating systems. (Fourth edition / Andrew S. Tanenbaum, Herbert Bos, Vrije Universiteit, Amsterdam, The Netherlands.). Pearson.


Describe time-sharing and space-sharing in operating systems. What resources are shared in time/space? Please also cite the page and specific quotes from where you found the answers.

What is a trap instruction? Explain its use in operating systems.

A trap instruction is a special instruction that is used to switch from user mode to kernel mode and start the operating system. When a user program needs to access services provided by the operating system, it makes a system call, which triggers a trap instruction. The trap instruction transfers control from user mode to kernel mode, where the operating system can perform the requested service on behalf of the user program.

In other words, when a user program needs to perform an operation that requires privileged access (such as accessing hardware devices or modifying system settings), it cannot do so directly in user mode. Instead, it must make a request to the operating system by issuing a system call. The trap instruction is used by the processor to transfer control from user mode to kernel mode and start executing code in the operating system's privileged execution environment.

Once the requested service has been performed by the operating system, control is returned back to the user program at the instruction following the system call. This allows user programs to access services provided by the operating system without having direct access to privileged resources.

(Page 22: Operating System Services)

Describe what a CPU pipeline is. What benefits do users receive from pipelining?

A CPU pipeline is a technique used by modern CPUs to improve performance by allowing the processor to execute multiple instructions simultaneously. In a pipeline, the processor is divided into separate stages, each of which performs a specific operation on an instruction. As one instruction moves through the pipeline, the next instruction can enter the pipeline and begin processing.

For example, in a three-stage pipeline, while one instruction is being executed in the third stage, another instruction can be decoded in the second stage and a third instruction can be fetched from memory in the first stage. This allows multiple instructions to be processed simultaneously and improves overall throughput.

The benefits of pipelining for users include faster execution times for programs and improved system responsiveness. By allowing multiple instructions to be processed simultaneously, pipelining can significantly reduce the time it takes to execute complex programs or perform resource-intensive tasks

(Page 21: Pipelining)

What is i-Node? Explain its use in operating systems.

An inode contains metadata about a file, such as its owner, permissions, size, and location on disk. When a file is created or modified, the inode is updated with the new information. Inodes are used by the operating 
system to locate and access files on disk.

In summary, an inode is a data structure used by Unix-based file systems to store metadata about files and directories. It is an important component of the file system that allows the operating system to manage files efficiently.

(Page 91: File System Implementation)
