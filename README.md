# Buffer Pool Database Management System 

This project implements a database management system (DBMS) that relies on a **buffer pool manager** to manage interactions between memory and disk for blocks of records. The system supports commands to retrieve, update, pin/unpin database blocks and exit the program.

### Key Features
- **Buffer Pool Manager**: Optimized memory usage for blocks of data by storing frequently accessed blocks in memory.
- **Command Interface**: Use basic commands like `GET`, `SET`, `PIN`, `UNPIN`, and `EXIT` to interact with the database.
- **Handles Disk Storage**: Stores and retrieves records and blocks using disk files.

---
# SECTION I
---

## Compilation Instructions
(A compiled executable is already provided, refer to next section)

To compile the program, the following dependencies and tools are required:
- A **C++20** compliant compiler (e.g., GCC 10+, Clang 11+, MSVC 2019+).
- **CMake** version 3.30 or later.

### Steps to Compile:
1. **Clone the Repository**:
   ```bash
   cd <project_directory>
   cmake --build cmake-build-debug --target hola -j 8
   ```

4. The output executable `hola` will be located in the `cmake-build-debug` directory.

---

## Run Instructions

Once the executable `hola` has been compiled, you can run the program as follows:

```bash
./cmake-build-debug/hola <buffer_pool_size>
```

- **`<buffer_pool_size>`**: An integer greater than 0 specifying the maximum number of frames (blocks) available in the buffer pool.

---

## Commands

The program supports the following commands, which can be entered interactively once the program is running.

| Command                           | Example                               | Description                                                                                      |
|-----------------------------------|---------------------------------------|--------------------------------------------------------------------------------------------------|
| `GET <Record_number>`             | `GET 001`                             | Retrieves the record identified by `<Record_number>` from memory or disk.                       |
| `SET <Record_number> <Record>`    | `SET 200 F04-RecordValue....`         | Updates the record identified by `<Record_number>` with the 40-byte `<Record>` value.           |
| `PIN <Block_number>`              | `PIN 10`                              | Pins the block `<Block_number>` in the buffer pool, preventing it from being evicted.           |
| `UNPIN <Block_number>`            | `UNPIN 10`                            | Unpins the block `<Block_number>`, allowing it to be evicted if necessary.                      |
| `EXIT`                            | `EXIT`                                | Exits the program.                                                                              |

> **Note**:
> - `<Record_number>`: A 3-digit value (e.g., 001, 200).
> - `<Record>`: A string with exactly **40 characters**.

---

# SECTION II
## TEST RESULTS

The program was tested against testcase_commands_and_output.txt, and all the commands worked properly.

---

# SECTION III

## Files and Structure

### 1. **Source Code**
The project files are organized as follows:

| File                     | Description                                                                                       |
|--------------------------|---------------------------------------------------------------------------------------------------|
| `main.cpp`               | Contains the entry point of the program, command parsing, and program execution flow.            |
| `BufferPool.h/.cpp`      | Implementation of the buffer pool, responsible for managing memory-to-disk interactions.          |
| `DiskStorageConfig.h`    | Defines constants (`RECORD_SIZE`, `DISK_BLOCK_SIZE`) and data structures (**Record**, **Block**). |
| `Frame.h/.cpp`           | Encapsulation of a buffer pool frame. Tracks block ownership, pinning, and dirty states.          |
| `CommandParser.h/.cpp`   | Parses user commands into executable actions.                                                     |
| `RequestResults.h`       | Defines the possible command results (success or error variants).                                 |
| `ResultDisplayer.h/.cpp` | Displays command results to the user.                                                             |

---
