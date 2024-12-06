# Handle Checker for Process References

## Overview
This tool allows you to detect and display handles that other processes hold to a specified target process on a Windows machine. The program enumerates system handles and identifies processes that have open handles referencing the target process. It can be useful for debugging, security analysis, or understanding process interactions.

## Features
- Enumerates system handles across all running processes.
- Identifies and filters handles that reference a target process.
- Displays detailed information about each process holding a handle, including:
  - Process name
  - Process ID (PID)
  - Parent process ID
  - Thread count
  - Handle value
  - Handle access rights

## Requirements
- Windows OS (tested on Windows 10 and later).
- A compiler that supports C++11 or later (e.g., Visual Studio).
- Administrator privileges may be required to access some process details and handle information.

## Installation
1. Clone or download the repository to your local machine.
2. Open the project in your preferred C++ IDE (e.g., Visual Studio).
3. Build the project (e.g., press `F7` in Visual Studio).
4. Ensure that you have the necessary permissions to access process information by running the program with Administrator privileges.

## Usage

### Running the Program
1. **Launch the program** with Administrator privileges (right-click the executable and select "Run as Administrator").
2. **Input the target process ID (PID)** when prompted. The program will then search for all processes that hold open handles to this target process.

```text
Enter target process ID: [Your Process ID]





Enter target process ID: 1234

Handle 5678 from process 4321 is referencing PID 1234
Process Details:
Name: target_process.exe
PID: 4321
Path: C:\path\to\process\executable.exe
Parent PID: 6789
Thread Count: 12
Handle Value: 5678
Handle Access: 0x1F0003

This version is now formatted with proper headings, code blocks, and markdown for easy reading and copying. Just paste it into your project’s `README.md` file, and you're good to go!

