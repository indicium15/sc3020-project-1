# SC3020 Databse System Principles Project 1
## Overview
This project focuses on the design and implementation of a database system with optimized storage records and B+ Tree indexing for managing a large dataset of NBA games. Our team has worked on reducing the size of stored records, optimizing disk I/O operations, and ensuring efficient query processing using a B+ Tree structure.

## Team Members
Sethi Aryan
Jadhav Chaitanya
Gupta Tushar
Lin Run Yu

## Features
Record Component Design: Optimizes record storage by using smaller data types and storing calculated offsets.
Storage Component Design: Implements an unspanned record storage system for efficient disk I/O.
B+ Tree Design: Efficient indexing and retrieval capabilities using a B+ Tree structure.

## Experiments and Results
We conducted various experiments to test the efficiency of our storage and retrieval system, including bulk loading, search, and deletion operations in the B+ Tree structure. These experiments demonstrated the effectiveness of our optimizations in terms of reduced running time and disk I/O operations.


## Installation Instructions

1. Download `clang++` for compilation on MacOS / Linux and `Windows Subsystem for Linux` on Windows.
2. Clone this repository.
3. Navigate to `build` folder and run the following command:

Compile Command: `clang++ -std=c++11 -stdlib=libc++  ../src/main.cpp ../src/record.cpp ../src/storage.cpp ../src/bplustree.cpp -o main`

4. Run the `main` file generated.

## Output Files
1. `experiments.txt` : contains all the experiment data required from the project.
2. `output.txt` : contains all cout statements used for debugging data generated by the program.

Please refer to the comments in the code to get a better understanding of our implementations.

## Acknowledgments
We thank our course instructors and TAs for their guidance throughout the project.
