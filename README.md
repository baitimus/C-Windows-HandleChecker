Handle Checker for Process References
Overview
This tool allows you to detect and display handles that other processes hold to a specified target process on a Windows machine. The program enumerates system handles and identifies processes that have open handles referencing the target process. It can be useful for debugging, security analysis, or understanding process interactions.

Features
Enumerates system handles across all running processes.
Identifies and filters handles that reference a target process.
Displays detailed information about each process holding a handle, including:
Process name
Process ID (PID)
Parent process ID
Thread count
Handle value
Handle access rights
Requirements
Windows OS (tested on Windows 10 and later).
A compiler that supports C++11 or later (e.g., Visual Studio).
Administrator privileges may be required to access some process details and handle information.
Installation
Clone or download the repository to your local machine.
Open the project in your preferred C++ IDE (e.g., Visual Studio).
Build the project (e.g., press F7 in Visual Studio).
Ensure that you have the necessary permissions to access process information by running the program with Administrator privileges.
Usage
Running the Program
Launch the program with Administrator privileges (right-click the executable and select "Run as Administrator").
Input the target process ID (PID) when prompted. The program will then search for all processes that hold open handles to this target process.
text
Copy code
Enter target process ID: [Your Process ID]
The program will then enumerate system handles, check for open handles referencing the target process, and display detailed information about any matching processes.
Sample Output
text
Copy code
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
Available Information
Handle Value: The numeric identifier of the handle held by the process.
Handle Access: A hexadecimal value indicating the access rights granted to the handle.
Process Details: Name, path, parent PID, and thread count for each process that holds a reference to the target process.
Functionality Details
Key Functions:
GetHandles: Retrieves all system handles using the NtQuerySystemInformation function. This function queries all handles in the system and returns a list of SYSTEM_HANDLE structures.
DetectOpenHandlesToProcess: Filters the handles and checks if any process holds a handle that references the target process. It uses DuplicateHandle to verify if the handle corresponds to the target process.
GetDetailedProcessInfo: Retrieves detailed information about a specific process, such as its name, path, parent PID, and thread count.
GetProcessName: Retrieves the name of a process by its PID using the EnumProcessModules function.
Dependencies:
Windows API: The program relies on various Windows API functions (OpenProcess, EnumProcessModules, NtQuerySystemInformation, DuplicateHandle, etc.).
Psapi.h: For process and module enumeration.
TlHelp32.h: For process snapshot functionality (e.g., CreateToolhelp32Snapshot).
ntstatus.h: For NT status codes, specifically used with NtQuerySystemInformation.
Troubleshooting
Access Denied: If you receive access errors, ensure you run the program with Administrator privileges. Some processes require elevated access to view their handle information.
No Results Found: If no processes are displayed as referencing the target process, ensure that the target process has open resources (such as windows, files, etc.) that other processes could hold handles to.
Error: NtQuerySystemInformation: If the system call fails, ensure your environment is set up correctly and that the ntdll.dll library is accessible.
Contributing
Feel free to contribute by submitting issues or pull requests. If you have improvements or bug fixes, don't hesitate to get involved.

License
This project is licensed under the MIT License - see the LICENSE file for details.
