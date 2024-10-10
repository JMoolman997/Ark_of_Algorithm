TRANSPORT GENIE - A CLI Tool for Solving Transportation Problems
===============================================================

Transport Genie is a command-line tool that uses Vogel's Approximation Method (VAM) 
to solve transportation problems. It helps you find an optimal transportation plan 
based on user-provided supply, demand, and cost matrices.

It currently supports three methods of solving transportation problems:

- **Vogel's Approximation Method (VAM)**
- **North-West Corner Method (NWCM)**
- **Least Cost Method (LCM)**

The transportation problem involves minimizing the cost of transporting goods from supply nodes to demand nodes based on user-provided supply, 
demand, and cost matrices. The tool also balances the problem by adding dummy nodes when necessary.

---------------------------------------------------------------
HOW TO USE TRANSPORT GENIE
---------------------------------------------------------------

1. **Compile the Program**

   First, ensure you have `gcc` and `make` installed on your system.
   
   To compile the program, navigate to the project root directory and run:
   
   make
   
   This will create an executable called `transport_genie` inside the `bin/` directory.

2. **Running the Program**

   Once compiled, run the program with the following command:
   
   ./bin/transport_genie

3. **Input Format**

   When prompted by the program, provide the following inputs:

   - **Supply Vector**: A list of supply values for each supply point, entered as comma-separated integers.
   - **Demand Vector**: A list of demand values for each demand point, entered as comma-separated integers.
   - **Cost Matrix**: A matrix of costs, where each row corresponds to a supply point and each column corresponds to a demand point. 
                      Each row is entered as a comma-separated list of integers.

   Example input format for a problem with 3 supply points and 3 demand points:

   - Supply Vector: 20,30,25
   - Demand Vector: 10,35,30
   - Cost Matrix:
     - Row 1: 8,6,10
     - Row 2: 9,12,13
     - Row 3: 14,9,16

4. **Iteration Printing Option**

   After entering the cost matrix, the program will ask if you want to print each iteration of the allocation process. 
   Answer `y` for yes or `n` for no.

5. **Output**

   After solving, the program will display:
   
   - The final allocation matrix, showing how much is transported between each supply and demand point.
   - The total cost of the transportation plan.

---------------------------------------------------------------
EXAMPLES
---------------------------------------------------------------

Example input and output:

1. **Input:**
   
   Supply: 20,30,25
   
   Demand: 10,35,30
   
   Cost Matrix:
   - Row 1: 8,6,10
   - Row 2: 9,12,13
   - Row 3: 14,9,16

2. **Output:**

   Transportation Plan (Allocation Matrix):
       D1  D2  D3  
   S1  10 10  0   
   S2   0  0  30   
   S3   0 25  0   

   Total Cost: 470

---------------------------------------------------------------
TROUBLESHOOTING
---------------------------------------------------------------

- If the program fails to compile, ensure that `gcc` and `make` are properly installed on your system.
- If you encounter an error related to memory allocation, ensure that your system has enough memory to handle the problem size.
- Ensure all input data is correctly formatted (comma-separated lists for vectors and matrices).

---------------------------------------------------------------
DISCLAIMER
---------------------------------------------------------------

Transport Genie is provided as-is without any guarantees of accuracy, performance, 
or fitness for a particular purpose. It is a tool designed for educational and 
experimental purposes, and the author assumes no responsibility for any issues 
that may arise from its use.

Use this tool at your own risk. Make sure to validate the results independently 
if used for critical purposes.

---------------------------------------------------------------
AUTHOR
---------------------------------------------------------------

Developed by Moolman997 as part of the Ark of Algorithm project.
