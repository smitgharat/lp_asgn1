/********************************************************************************************************************
CSL 316 - Language Processors - Assignment 1 - Semester 6th Year 3rd

Programmer: Smit Gharat
Batch:   R5
Professor: Dr. Manish Kurhekar
Date Due:  24 Jan 2024

Purpose:   The purpose of this assignment is to implement a basic memory management system in C++, 
           utilizing linked lists to manage allocated and free memory blocks. The program interprets 
           transactions from an input file, including memory allocation, deallocation, and variable 
           assignments, while also incorporating memory compaction to reduce fragmentation.
*********************************************************************/

#include<bits/stdc++.h>
using namespace std;
#define TOTAL_MEMORY 64 * 1024 * 1024

/******************************************************************************************************************
Structure: MemoryBlock
Use: Defines a structure representing a memory block with details such as size, start address, reference count, and a next pointer.
Members:
    - size (int): Size of the memory block in bytes.
    - start_address (int): Starting address of the memory block.
    - reference_count (int): Number of references to this memory block.
    - next (MemoryBlock*): Pointer to the next node in the linked list of memory blocks.
*******************************************************************************************************************/
struct MemoryBlock 
    {
    int size;               // Size of the memory block in bytes
    int start_address;      // Starting address of the memory block
    int reference_count;    // Number of references to this memory block
    MemoryBlock* next;      // Pointer to the next node in the linked list

    MemoryBlock(int size, int start_address, MemoryBlock* next = nullptr)
        : size(size), start_address(start_address), reference_count(1), next(next) 
            {   }
    };

/******************************************************************************************************************
Class: MemoryManager
Use: Manages memory allocation and deallocation using a simple memory block structure.
Members:
    - memory_chunk (int): Total size of the memory managed by the MemoryManager.
    - used_blocks (MemoryBlock*): Pointer to the head of the linked list of used memory blocks.
    - free_blocks (MemoryBlock*): Pointer to the head of the linked list of free memory blocks.
Public Member Functions:
    1. MemoryManager(int memory_chunk)
        - Constructor for initializing the MemoryManager with a specified memory chunk size.
    2. int allocateBlock(int size)
        - Allocates a block of memory with the given size from the free memory blocks.
    3. int allocate(int size)
        - Allocates a block of memory with the given size, trying to compact memory if no sufficiently large block is found.
    4. void deallocate(int start_address)
        - Deallocates the memory block at the specified start address.
    5. void compact_memory()
        - Compacts the memory by moving used blocks closer together and merging adjacent free blocks.
    6. void print_memory_status()
        - Prints the current status of used and free memory blocks.
    7. ~MemoryManager()
        - Destructor to clean up allocated memory blocks when the MemoryManager object is destroyed.
Private Member Functions:
    - Nothing
Notes:
    - Memory allocation is performed by the `allocateBlock` function, and if no sufficiently large block is found, it tries to compact memory before retrying.
    - Memory deallocation is performed by the `deallocate` function, which adjusts reference counts and moves blocks between used and free lists.
    - Memory compaction is triggered by the `compact_memory` function, which rearranges used and free blocks to reduce fragmentation.
    - The `print_memory_status` function outputs details of used and free memory blocks to the console.
*******************************************************************************************************************/

class MemoryManager {
public:
    int memory_chunk;
    MemoryBlock* used_blocks;
    MemoryBlock* free_blocks;
/******************************************************************************************************************
Constructor: MemoryManager
Use: Initializes a MemoryManager object with the specified memory chunk size.
Arguments:
    - memory_chunk (int): The total size of the memory managed by the MemoryManager.
Members:
    - memory_chunk (int): Total size of the memory managed by the MemoryManager.
    - used_blocks (MemoryBlock*): Pointer to the head of the linked list of used memory blocks.
    - free_blocks (MemoryBlock*): Pointer to the head of the linked list of free memory blocks.
Initialization:
    - Allocates the initial free memory block covering the entire memory chunk.
    - Sets used_blocks to nullptr as there are no allocated blocks initially.
Notes:
    - This constructor is called when creating a MemoryManager object to manage a specific amount of memory.
    - The allocated memory is initially considered as a single free block, covering the entire memory chunk.
*******************************************************************************************************************/

    MemoryManager(int memory_chunk) : memory_chunk(memory_chunk) 
        {
        free_blocks = new MemoryBlock(memory_chunk, 0);
        used_blocks = nullptr;
        }

/******************************************************************************************************************
Function: allocateBlock
Use: Allocates a block of memory with the given size from the free memory blocks.
Arguments:
    - size (int): Size of the memory block to be allocated.
Returns:
    - Start address of the allocated memory block, or -1 if allocation fails.
Functionality:
    - Searches the linked list of free memory blocks to find a block of sufficient size.
    - If a suitable block is found:
        - Allocates a new memory block in the used memory blocks list.
        - Adjusts the free block's start address and size accordingly.
        - Deletes the free block if its size becomes zero.
    - Returns the start address of the allocated block or -1 if allocation fails.
Notes:
    - This function is called by the `allocate` function when a new memory block needs to be allocated.
*******************************************************************************************************************/

    int allocateBlock(int size) 
        {
        MemoryBlock* current_free_block = free_blocks;
        MemoryBlock* prev_free_block = nullptr;

        while (current_free_block != nullptr) 
            {
            if (current_free_block->size >= size) 
                {
                int start_address = current_free_block->start_address;
                MemoryBlock* allocated_block = new MemoryBlock(size, start_address);
                allocated_block->next = used_blocks;
                used_blocks = allocated_block;

                current_free_block->start_address += size;
                current_free_block->size -= size;

                if (current_free_block->size == 0) 
                    {
                    if (prev_free_block == nullptr) 
                        {
                        free_blocks = current_free_block->next;
                        } 
                    else 
                        {
                        prev_free_block->next = current_free_block->next;
                        }
                    delete current_free_block;
                    }

                return start_address;
                }

            prev_free_block = current_free_block;
            current_free_block = current_free_block->next;
            }

        return -1;
        }

/******************************************************************************************************************
Function: allocate
Use: Allocates a block of memory with the given size, trying to compact memory if no sufficiently large block is found.
Arguments:
    - size (int): Size of the memory block to be allocated.
Returns:
    - Start address of the allocated memory block, or -1 if allocation fails.
Functionality:
    - Calls the `allocateBlock` function to attempt memory allocation.
    - If allocation fails, it tries to compact memory using the `compact_memory` function and retries the allocation.
    - If allocation still fails, outputs an error message to the standard error stream.
    - Returns the start address of the allocated block or -1 if allocation fails.
Notes:
    - This function is the primary interface for allocating memory in the MemoryManager.
*******************************************************************************************************************/

    int allocate(int size) 
        {
        int start_address = allocateBlock(size);

        // If no sufficiently large block is found, try compacting memory
        if (start_address == -1) 
            {
            compact_memory();
            start_address = allocateBlock(size);
            }

        if (start_address == -1) 
            {
            cout << "Error: Unable to allocate memory of size " << size << endl;
            }

        return start_address;
        }

/******************************************************************************************************************
Function: deallocate
Use: Deallocates the memory block at the specified start address.
Arguments:
    - start_address (int): Start address of the memory block to be deallocated.
Returns:   
    - nothing
Functionality:
    - Searches the linked list of used memory blocks to find the block with the given start address.
    - Decreases the reference count of the found block.
    - If the reference count becomes zero:
        - Removes the block from the used memory blocks list.
        - Adds the block to the free memory blocks list.
    - Outputs an error message to the standard error stream if the block is not found.
Notes:
    - This function is responsible for deallocating memory, adjusting reference counts, and managing the used and free memory block lists.
*******************************************************************************************************************/

    void deallocate(int start_address) 
        {
        MemoryBlock* current_block = used_blocks;
        MemoryBlock* prev_block = nullptr;

        while (current_block != nullptr) 
            {
            if (current_block->start_address == start_address) 
                {
                current_block->reference_count--;

                if (current_block->reference_count == 0) 
                    {
                    MemoryBlock* freed_block = current_block;
                    if (prev_block == nullptr) 
                        {
                        used_blocks = current_block->next;
                        } else 
                        {
                        prev_block->next = current_block->next;
                        }

                    freed_block->next = free_blocks;
                    free_blocks = freed_block;
                    }

                return;
                }

            prev_block = current_block;
            current_block = current_block->next;
            }

        cout << "Error: Block at address " << start_address << " not found for deallocation." << endl;
        }

/******************************************************************************************************************
Function: compact_memory
Use: Compacts the memory by moving used blocks closer together and merging adjacent free blocks.
Functionality:
    - Iterates through the linked list of used memory blocks and adjusts their start addresses based on the available free space.
    - Updates the start address of corresponding free blocks to reflect the changes made to used blocks.
    - Checks for adjacent free blocks and merges them if possible.
Arguments:
    - Nothing
Returns:
    - Nothing
Notes:
    - This function is called when memory needs to be compacted, typically during the allocation process when no sufficiently large block is found.
    - It aims to reduce fragmentation by moving used blocks closer together and merging adjacent free blocks.
*******************************************************************************************************************/
    
    void compact_memory() 
        {
        MemoryBlock* current_used = used_blocks;
        MemoryBlock* current_free = free_blocks;

        int prev = current_free->start_address;

        while (current_used != nullptr) 
            {
            // Find the appropriate free block for the used block
            while (current_free != nullptr && current_free->start_address != prev) 
                {
                current_free = current_free->next;
                }

            if (current_free != nullptr) 
                {
                current_used->start_address = prev;
                current_free->start_address = prev + current_used->size;

                // Check if the next free block can be merged
                if (current_free->next != nullptr && current_free->start_address + current_free->size == current_free->next->start_address) 
                    {
                    current_free->size += current_free->next->size;
                    MemoryBlock* temp = current_free->next;
                    current_free->next = current_free->next->next;
                    delete temp; // Remove the merged free block
                    }

                prev = current_free->start_address + current_free->size;
                } 

            // Move to the next used block
            current_used = current_used->next;
            }
        }

/******************************************************************************************************************
Function: print_memory_status
Use: Prints the current status of used and free memory blocks to the standard output.
Functionality:
    - Outputs information about used memory blocks, including their start address, size, and reference count.
    - Outputs information about free memory blocks, including their start address and size.
    - Utilizes cout for printing to the standard output.
Arguments:
    - Nothing
Returns:
    - Nothing
Notes:
    - This function is a diagnostic tool to observe the current state of used and free memory blocks.
    - It helps in monitoring memory usage and checking the effectiveness of memory allocation and deallocation processes.
*******************************************************************************************************************/

    void print_memory_status() 
        {
        cout << "Used Blocks:\n";
        MemoryBlock* current_block = used_blocks;
        while (current_block != nullptr) 
            {
            cout << "Address: " << current_block->start_address << ", Size: " << current_block->size
                 << ", Reference Count: " << current_block->reference_count << "\n";
            current_block = current_block->next;
            }

        cout << "\nFree Blocks:\n";
        current_block = free_blocks;
        while (current_block != nullptr) 
            {
            cout << "Address: " << current_block->start_address << ", Size: " << current_block->size << "\n";
            current_block = current_block->next;
            }
        }

/******************************************************************************************************************
Destructor: ~MemoryManager
Use: Cleans up allocated memory blocks when the MemoryManager object is destroyed.
Functionality:
    - Iterates through the linked list of used memory blocks, deletes each block, and moves to the next block.
    - Iterates through the linked list of free memory blocks, deletes each block, and moves to the next block.
    - Frees the memory used by both used and free memory block lists.
Arguments:
    - Nothing
Returns:
    - Nothing
Notes:
    - This destructor is automatically called when the MemoryManager object goes out of scope or is explicitly deleted.
    - It is responsible for releasing the memory used by all allocated memory blocks, preventing memory leaks.
*******************************************************************************************************************/

    ~MemoryManager() 
        {
        // Clean up allocated memory blocks
        MemoryBlock* current_block = used_blocks;
        while (current_block != nullptr) 
            {
            MemoryBlock* next_block = current_block->next;
            delete current_block;
            current_block = next_block;
            }

        current_block = free_blocks;
        while (current_block != nullptr) 
            {
            MemoryBlock* next_block = current_block->next;
            delete current_block;
            current_block = next_block;
            }
        }
    };
/******************************************************************************************************************
Function: process_transaction
Use: Processes a transaction string, performing memory management operations based on the specified actions.
Arguments:
    - transaction (const string&): The input string representing a memory management transaction.
    - memory_manager (MemoryManager&): A reference to the MemoryManager object responsible for managing memory.
    - variables (unordered_map<string, int>&): A mapping of variable names to their corresponding memory block start addresses.
Functionality:
    - Parses the input transaction string to extract variable name, action, and size (if applicable).
    - Performs memory management operations based on the specified action:
        - "allocate": Allocates a memory block of the specified size and associates it with the given variable.
        - "free": Deallocates the memory block associated with the specified variable.
        - Variable assignment: Copies the memory block address from one variable to another, increasing the reference count.
    - Outputs error messages to the standard error stream for unsupported operations or incorrect syntax.
Arguments:
    - transaction (const string&): The input transaction string to be processed.
    - memory_manager (MemoryManager&): A reference to the MemoryManager object for memory management operations.
    - variables (unordered_map<string, int>&): A mapping of variable names to their corresponding memory block start addresses.
Returns:
    - Nothing
Notes:
    - This function is a central part of the memory management system, interpreting and executing transactions.
    - It handles memory allocation, deallocation, and variable assignments based on the specified actions.
    - Errors and unsupported operations are reported to the standard error stream.
*******************************************************************************************************************/

void process_transaction(const string& transaction, MemoryManager& memory_manager, unordered_map<string, int>& variables) 
    {
    istringstream iss(transaction);
    string variable, 
           action, 
           equal_sign, 
           operand;
    int size;

    iss >> variable >> equal_sign >> action;

    if (equal_sign == "=" && action == "allocate") 
        {
        iss >> size;
        int start_address = memory_manager.allocate(size);
        if (start_address != -1) 
            {
            variables[variable] = start_address;
            }
        } 
    else if (variable == "free") 
        {
        memory_manager.deallocate(variables[equal_sign]);
        } 
    else if (equal_sign == "=" && action != "allocate") 
        {
        // Handle variable assignment: b = a
        string source_variable;
        iss >> source_variable;
        variables[variable] = variables[source_variable];

        // Increase reference count for the allocated memory
        MemoryBlock* current_block = memory_manager.used_blocks;
        while (current_block != nullptr) 
            {
            if (current_block->start_address == variables[source_variable]) 
                {
                current_block->reference_count++;
                break;
                }
            current_block = current_block->next;
            }

        if (current_block == nullptr) 
            {
            cout << "Error: Block at address " << variables[source_variable]
                 << " not found for reference count increase." << endl;
            }
        } 
    else 
        {
        cout << "Error: Unsupported operation or incorrect syntax: " << transaction << endl;
        }
    }

/******************************************************************************************************************
Function: main
Use: Entry point of the program, responsible for initializing memory management, processing transactions, and printing
     the final memory status to an output file.
Functionality:
    - Creates a MemoryManager object with a specified total memory size.
    - Initializes an unordered_map to store variable names and their corresponding memory block start addresses.
    - Attempts to open input and output files, displaying error messages if unsuccessful.
    - Reads each line from the input file, processes transactions using the MemoryManager and variables map.
    - Redirects cout to the output file to print the final memory status.
    - Resets cout to its original buffer after printing.
    - Closes input and output files.
Parameters:
    - None
Returns:
    - 0 if the program executes successfully, 1 otherwise.
Notes:
    - The program processes memory management transactions from an input file and outputs the final memory status to
      an output file.
    - Errors during file operations are reported to the standard error stream.
*******************************************************************************************************************/

int main() 
    {
    MemoryManager memory_manager(TOTAL_MEMORY);  // Create MemoryManager object with specified total memory size
    unordered_map<string, int> variables;        // Initialize map to store variable names and memory block addresses

    ifstream input_file("input.txt");           // Open input file for reading
    ofstream output_file("output.txt");         // Open output file for writing

    if (!input_file.is_open()) 
        {
        cout << "Error: Unable to open input file." << endl;  // Display error if input file cannot be opened
        return 1;                                              // Return error code
        }

    if (!output_file.is_open()) 
        {
        cout << "Error: Unable to open output file." << endl; // Display error if output file cannot be opened
        return 1;                                              // Return error code
        }

    string transaction;
    while (getline(input_file, transaction)) 
        {
        process_transaction(transaction, memory_manager, variables);  // Process each transaction from input file
        }

    // Redirect output to the file
    streambuf *coutbuf = cout.rdbuf();   // Save old cout buffer
    cout.rdbuf(output_file.rdbuf());     // Redirect cout to output_file

    memory_manager.print_memory_status();   // Print memory status to the file

    cout.rdbuf(coutbuf);    // Reset cout to the original buffer

    input_file.close();     // Close input file
    output_file.close();    // Close output file

    return 0;  // Return success code
    }

