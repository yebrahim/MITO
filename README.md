# MITO - Multithreaded Task Overlapper for Parallel I/O Systems

MITO is a Multithreaded Task Overlapper, initially created to facilitate the overlapping of I/O and compute in multithreaded environments, especially for parallel file systems, such as the IBM General Parallel File System.

The purpose of this library and code is educational in the first place. I wanted to investigate and learn more about the POSIX threads and MPI (this version still does not support multiprocessing with MPI, I'm working on it).

## Design

The design of the library aims at letting the user specify the memory limit allowed for the use of the library, and point it to an input file. The library reads the file in the form of fixed-size chunks (the user can change the chunk size), and for each chunk in memory, passes it to a function that is also specified by the user to process this chunk.

The library maintains three queues of chunks in memory, namely: empty queue, processable queue, and writable queue. As the names indicate, the empty queue is a queue of empty chunks that are filled with data read from the file system. After a chunk is filled with data, it is passed to the processable queue, where a different thread (of MPI process, or a remote thread on a different MPI process) can call the user-specified routine to process it. It then puts it on the writable queue, where a third thread can write it out to the filesystem. The purpose of this design is to automate pipelining and make the I/O and compute steps run in parallel.

## Usage
The proper way to use this code is to do the following:

* Put a `routine.cpp` file in the same directory, and implement a function that takes a pointer to a chunk, and runs the processing logic on it.
* In your `main` function (or wherever you're calling the library), you need to specify the input and output paths to the `init()` function. You should also call the `set_routine()` function to point it at your processing function.
* You can set the number of input, compute, and output threads before calling the library's `init()` by setting these variables: `NUM_INP_THREADS`, `NUM_COMP_THREADS`, and `NUM_OUTP_THREADS`.
* You can change the maximum number of chunks the library is allowed to allocate in memory by changing the parameter `NUM_CHUNKS`.
* For a sample run, create a binary input file by running the binary generator file in the data directory, specifying the number of gigabytes for the file size and the file name. Then you can run make, then:

        ./main.o ../data/data.in ../data/data.out
  
* If you want to control the thread counts from the command line, you can specify a number for each of the three types of thread: input, compute, and output. For example, you can run:

        ./main.o ../data/data.in ../data/data.out 1 2 0
        for one input, two compute, and not output threads.

Please note that when running on a filesystem that does not support parallel I/O, having input and output threads at the same time will slow you down because of extensive seeking and inability to properly use the disk buffering. It's better to change the pipeline in this case to delay all output till the end.
