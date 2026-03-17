# 🚀 Custom Virtual File System (CVFS)

## 📌 Overview
The Custom Virtual File System (CVFS) is a simulation of a UNIX-like file system implemented in C.  
It demonstrates core operating system concepts such as inode management, file handling, and memory management.

This project mimics how real file systems internally manage files using structures like superblock, inode list, and file descriptor table.

---

## 🧠 Key Concepts Implemented
- Inode Data Structure
- Superblock Management
- File Descriptor Table (UFDT)
- Dynamic Memory Allocation
- File Permissions (Read / Write)
- Command Line Interface (CLI)

---

## ⚙️ Features
- 📁 Create new files  
- 📃 List all files (`ls`)  
- ✏️ Write data into files  
- 📖 Read data from files  
- ❌ Delete files (`unlink`)  
- 🔐 Permission-based access control  

---

## 🏗️ Project Structure
```
CVFS/
│
├── cvfs.h → Header file (structures, macros, declarations)
├── cvfs.c → Core file system logic
├── main.c → Command interpreter (CLI interface)

```
---

## 💻 How to Compile & Run

```bash
gcc main.c cvfs.c -o cvfs
./cvfs
🎮 Supported Commands
Command	Description
creat <name> <permission>	Create file
ls	List files
write <fd>	Write into file
read <fd> <size>	Read from file
unlink <name>	Delete file
help	Show commands
exit	Exit system
📸 Sample Execution
CVFS : > creat test 2
File created successfully with FD : 3

CVFS : > write 3
Enter data to write:
Hello World

CVFS : > read 3 5
Hello

CVFS : > ls
1   test   11
🔍 Internal Working

Uses a linked list of inodes (DILB) to store file metadata

Maintains Superblock for tracking free inodes

Uses UFDT (User File Descriptor Table) for open files

Each file contains:

Name

Size

Permission

Data buffer pointer

🚀 Future Enhancements

File open/close commands

Directory structure support

File seek functionality

Persistent storage (saving files on disk)

👩‍💻 Author

Pallavi Sable

⭐ Why This Project?

This project demonstrates strong understanding of:

Operating System concepts

Low-level memory management

Data structures (linked list)

System-level programming in C
