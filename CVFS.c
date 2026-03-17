#include "CVFS.h"

//////////////////////////////////////////////////////////
//
//  Global Variables Definition
//  (Actual memory allocation happens here)
//
//////////////////////////////////////////////////////////

PINODE head = NULL;          // Head of DILB (linked list of inodes)
SUPERBLOCK superobj;         // Superblock instance
UAREA uareaobj;              // User area instance

//////////////////////////////////////////////////////////
//
//  Function Name : InitialiseUAREA
//  Description   : Initializes user file descriptor table
//
//////////////////////////////////////////////////////////

void InitialiseUAREA()
{
    strcpy(uareaobj.ProcessName,"CVFS_Process");

    // Initially all file descriptors are empty
    for(int i = 0; i < MAXOPENFILES; i++)
    {
        uareaobj.UFDT[i] = NULL;
    }

    printf("CVFS : UAREA initialized successfully\n");
}

//////////////////////////////////////////////////////////
//
//  Function Name : InitialiseSuperBlock
//  Description   : Initializes total and free inodes
//
//////////////////////////////////////////////////////////

void InitialiseSuperBlock()
{
    superobj.TotalInodes = MAXINODE;
    superobj.FreeInodes = MAXINODE;

    printf("CVFS : SuperBlock initialized successfully\n");
}

//////////////////////////////////////////////////////////
//
//  Function Name : CreateDILB
//  Description   : Creates linked list of inodes (DILB)
//
//////////////////////////////////////////////////////////

void CreateDILB()
{
    PINODE newn = NULL;
    PINODE temp = NULL;

    // Create MAXINODE number of inodes
    for(int i = 1; i <= MAXINODE; i++)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        // Initialize inode fields
        strcpy(newn->FileName, "");
        newn->InodeNumber = i;
        newn->FileSize = 0;
        newn->ActualFileSize = 0;
        newn->FileType = 0;
        newn->ReferenceCount = 0;
        newn->Permission = 0;
        newn->Buffer = NULL;
        newn->next = NULL;

        // Insert into linked list
        if(head == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
    }

    printf("CVFS : DILB created successfully\n");
}

//////////////////////////////////////////////////////////
//
//  Function Name : StartAuxillaryDataInitialization
//  Description   : Calls all initialization functions
//
//////////////////////////////////////////////////////////

void StartAuxillaryDataInitialization()
{
    InitialiseSuperBlock();     // Initialize superblock
    CreateDILB();               // Create inode list
    InitialiseUAREA();          // Initialize user area

    printf("CVFS : All data initialized successfully\n");
}

//////////////////////////////////////////////////////////
//
//  Function Name : IsFileExist
//  Description   : Checks whether file exists or not
//  Return        : true / false
//
//////////////////////////////////////////////////////////

bool IsFileExist(char *name)
{
    PINODE temp = head;

    // Traverse linked list
    while(temp != NULL)
    {
        // Check file name and type
        if((strcmp(name,temp->FileName) == 0) && (temp->FileType == REGULARFILE))
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

//////////////////////////////////////////////////////////
//
//  Function Name : CreateFile
//  Description   : Creates a new file
//  Return        : File descriptor / Error code
//
//////////////////////////////////////////////////////////

int CreateFile(char *name, int permission)
{
    PINODE temp = head;
    int i = 0;

    // Validation checks
    if(name == NULL || permission < 1 || permission > 3)
        return ERR_INVALID_PARAMETER;

    if(superobj.FreeInodes == 0)
        return ERR_NO_INODES;

    if(IsFileExist(name))
        return ERR_FILE_ALREADY_EXIST;

    // Find free inode
    while(temp != NULL)
    {
        if(temp->FileType == 0)
            break;
        temp = temp->next;
    }

    if(temp == NULL)
        return ERR_NO_INODES;

    // Find free file descriptor
    for(i = 3; i < MAXOPENFILES; i++)
    {
        if(uareaobj.UFDT[i] == NULL)
            break;
    }

    if(i == MAXOPENFILES)
        return ERR_MAX_FILES_OPEN;

    // Allocate file table
    uareaobj.UFDT[i] = (PFILETABLE)malloc(sizeof(FILETABLE));

    uareaobj.UFDT[i]->ReadOffset = 0;
    uareaobj.UFDT[i]->WriteOffset = 0;
    uareaobj.UFDT[i]->Mode = permission;
    uareaobj.UFDT[i]->ptrinode = temp;

    // Initialize inode data
    strcpy(temp->FileName,name);
    temp->FileSize = MAXFILESIZE;
    temp->ActualFileSize = 0;
    temp->FileType = REGULARFILE;
    temp->ReferenceCount = 1;
    temp->Permission = permission;
    temp->Buffer = (char *)malloc(MAXFILESIZE);

    superobj.FreeInodes--;

    return i;  // return file descriptor
}

//////////////////////////////////////////////////////////
//
//  Function Name : LsFile
//  Description   : Displays all existing files
//
//////////////////////////////////////////////////////////

void LsFile()
{
    PINODE temp = head;

    printf("\n----- File List -----\n");

    while(temp != NULL)
    {
        if(temp->FileType != 0)
        {
            printf("%d\t%s\t%d\n",
                temp->InodeNumber,
                temp->FileName,
                temp->ActualFileSize);
        }
        temp = temp->next;
    }
}

//////////////////////////////////////////////////////////
//
//  Function Name : UnlinkFile
//  Description   : Deletes a file
//  Return        : Success / Error
//
//////////////////////////////////////////////////////////

int UnlinkFile(char *name)
{
    if(name == NULL)
        return ERR_INVALID_PARAMETER;

    for(int i = 0; i < MAXOPENFILES; i++)
    {
        if(uareaobj.UFDT[i] != NULL)
        {
            if(strcmp(uareaobj.UFDT[i]->ptrinode->FileName, name) == 0)
            {
                // Free allocated memory
                free(uareaobj.UFDT[i]->ptrinode->Buffer);

                // Reset inode
                uareaobj.UFDT[i]->ptrinode->FileType = 0;
                uareaobj.UFDT[i]->ptrinode->ReferenceCount = 0;

                // Free file table
                free(uareaobj.UFDT[i]);
                uareaobj.UFDT[i] = NULL;

                superobj.FreeInodes++;

                return 0;
            }
        }
    }

    return ERR_FILE_NOT_EXIST;
}

//////////////////////////////////////////////////////////
//
//  Function Name : WriteFile
//  Description   : Writes data into file
//  Return        : Bytes written / Error
//
//////////////////////////////////////////////////////////

int WriteFile(int fd, char *data, int size)
{
    if(fd < 0 || fd >= MAXOPENFILES)
        return ERR_INVALID_PARAMETER;

    if(uareaobj.UFDT[fd] == NULL)
        return ERR_FILE_NOT_EXIST;

    if(uareaobj.UFDT[fd]->ptrinode->Permission < WRITE)
        return ERR_PERMISSION_DENIED;

    // Check available space
    if((MAXFILESIZE - uareaobj.UFDT[fd]->WriteOffset) < size)
        return ERR_INSUFFICIENT_SPACE;

    // Copy data into buffer
    strncpy(uareaobj.UFDT[fd]->ptrinode->Buffer +
            uareaobj.UFDT[fd]->WriteOffset,
            data, size);

    // Update offsets and size
    uareaobj.UFDT[fd]->WriteOffset += size;
    uareaobj.UFDT[fd]->ptrinode->ActualFileSize += size;

    return size;
}

//////////////////////////////////////////////////////////
//
//  Function Name : ReadFile
//  Description   : Reads data from file
//  Return        : Bytes read / Error
//
//////////////////////////////////////////////////////////

int ReadFile(int fd, char *data, int size)
{
    if(fd < 0 || fd >= MAXOPENFILES)
        return ERR_INVALID_PARAMETER;

    if(uareaobj.UFDT[fd] == NULL)
        return ERR_FILE_NOT_EXIST;

    if(uareaobj.UFDT[fd]->ptrinode->Permission < READ)
        return ERR_PERMISSION_DENIED;

    // Check available data
    if((uareaobj.UFDT[fd]->ptrinode->ActualFileSize -
        uareaobj.UFDT[fd]->ReadOffset) < size)
        return ERR_INSUFFICIENT_DATA;

    // Copy data from buffer
    strncpy(data,
        uareaobj.UFDT[fd]->ptrinode->Buffer +
        uareaobj.UFDT[fd]->ReadOffset,
        size);

    uareaobj.UFDT[fd]->ReadOffset += size;

    return size;
}

//////////////////////////////////////////////////////////
//
//  Function Name : DisplayHelp
//  Description   : Displays all supported commands
//
//////////////////////////////////////////////////////////

void DisplayHelp()
{
    printf("\nCommands:\n");
    printf("ls\ncreat <name> <permission>\n");
    printf("read <fd> <size>\nwrite <fd>\n");
    printf("unlink <name>\nexit\n");
}

//////////////////////////////////////////////////////////
//
//  Function Name : ManPageDisplay
//  Description   : Displays manual page (basic)
//
//////////////////////////////////////////////////////////

void ManPageDisplay(char Name[])
{
    printf("Manual not fully implemented\n");
}