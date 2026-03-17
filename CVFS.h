#ifndef CVFS_H
#define CVFS_H

//////////////////////////////////////////////////////////
//
//  Header Files
//
//////////////////////////////////////////////////////////

#include<stdio.h>      // For input-output functions
#include<stdlib.h>     // For dynamic memory allocation
#include<unistd.h>     // For system calls
#include<stdbool.h>    // For boolean datatype (true/false)
#include<string.h>     // For string operations

//////////////////////////////////////////////////////////
//
//  Macros (System Limits & Constants)
//
//////////////////////////////////////////////////////////

#define MAXFILESIZE 50      // Maximum size of a file
#define MAXOPENFILES 20     // Maximum number of files that can be opened
#define MAXINODE 5          // Total number of inodes in system

#define READ 1              // Read permission
#define WRITE 2             // Write permission

#define REGULARFILE 1       // Regular file type

//////////////////////////////////////////////////////////
//
//  Error Codes (Used for returning error status)
//
//////////////////////////////////////////////////////////

#define ERR_INVALID_PARAMETER -1       // Invalid input parameter
#define ERR_NO_INODES -2               // No free inode available
#define ERR_FILE_ALREADY_EXIST -3      // File already exists
#define ERR_FILE_NOT_EXIST -4          // File not found
#define ERR_PERMISSION_DENIED -5       // Permission denied
#define ERR_INSUFFICIENT_SPACE -6      // Not enough memory to write
#define ERR_INSUFFICIENT_DATA -7       // Not enough data to read
#define ERR_MAX_FILES_OPEN -8          // Maximum file limit reached

//////////////////////////////////////////////////////////
//
//  Structure Definitions
//
//////////////////////////////////////////////////////////

// INODE Structure (Represents a file metadata)
typedef struct Inode
{
    char FileName[20];      // Name of file
    int InodeNumber;        // Unique inode number
    int FileSize;           // Maximum file size
    int ActualFileSize;     // Current size of file
    int FileType;           // Type of file (regular/special)
    int ReferenceCount;     // Number of references
    int Permission;         // File permissions (Read/Write)
    char *Buffer;           // Pointer to file data
    struct Inode *next;     // Pointer to next inode
}INODE, *PINODE;

// FILETABLE Structure (Maintains file handling info)
typedef struct FileTable
{
    int ReadOffset;         // Current read position
    int WriteOffset;        // Current write position
    int Mode;               // Mode (Read/Write)
    PINODE ptrinode;        // Pointer to inode
}FILETABLE, *PFILETABLE;

// SUPERBLOCK Structure (Manages inode availability)
typedef struct SuperBlock
{
    int TotalInodes;        // Total inodes in system
    int FreeInodes;         // Available free inodes
}SUPERBLOCK;

// UAREA Structure (User Area / Process File Table)
typedef struct UAREA
{
    char ProcessName[20];           // Name of process
    PFILETABLE UFDT[MAXOPENFILES];  // User File Descriptor Table
}UAREA;

//////////////////////////////////////////////////////////
//
//  Global Variables (Declared using extern)
//
//////////////////////////////////////////////////////////

extern PINODE head;        // Head of inode linked list
extern SUPERBLOCK superobj;// Superblock object
extern UAREA uareaobj;     // User area object

//////////////////////////////////////////////////////////
//
//  Function Declarations
//
//////////////////////////////////////////////////////////

// Initialization functions
void InitialiseUAREA();                  // Initialize user area
void InitialiseSuperBlock();             // Initialize superblock
void CreateDILB();                      // Create linked list of inodes
void StartAuxillaryDataInitialization();// Initialize all components

// File handling functions
bool IsFileExist(char*);                // Check if file exists
int CreateFile(char*, int);             // Create new file
void LsFile();                          // List all files
int UnlinkFile(char*);                  // Delete file
int WriteFile(int, char*, int);         // Write data to file
int ReadFile(int, char*, int);          // Read data from file

// Utility functions
void DisplayHelp();                     // Show command help
void ManPageDisplay(char[]);            // Show manual page

#endif