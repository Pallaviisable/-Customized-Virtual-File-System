#include "CVFS.h"

//////////////////////////////////////////////////////////
//
//  Entry Point Function
//
//////////////////////////////////////////////////////////

int main()
{
    char *ptr = NULL;                 // Pointer for dynamic memory
    int ret = 0, fd = 0, count = 0;
    char command[4][80];              // To store user command parts
    char str[80], arr[1024];          // Buffers for input/output

    // Initialize CVFS system
    StartAuxillaryDataInitialization();

    printf("---------- Custom Virtual File System ----------\n");

    // Infinite loop for command execution
    while(1)
    {
        fflush(stdin);   // Clear input buffer

        // Display prompt
        printf("\nCVFS : > ");

        // Accept command from user
        fgets(str, 80, stdin);

        // Split command into tokens
        count = sscanf(str, "%s %s %s %s",
                       command[0], command[1],
                       command[2], command[3]);

        //////////////////////////////////////////////////////////
        //
        //  Command Handling Starts
        //
        //////////////////////////////////////////////////////////

        // EXIT command
        if(count == 1)
        {
            if(strcmp(command[0], "exit") == 0)
            {
                printf("Terminating CVFS...\n");
                break;
            }
            else if(strcmp(command[0], "ls") == 0)
            {
                LsFile();
            }
            else if(strcmp(command[0], "help") == 0)
            {
                DisplayHelp();
            }
            else
            {
                printf("Command not found\n");
            }
        }

        // CREATE FILE
        else if(count == 3)
        {
            if(strcmp(command[0], "creat") == 0)
            {
                ret = CreateFile(command[1], atoi(command[2]));

                if(ret >= 0)
                {
                    printf("File created successfully with FD : %d\n", ret);
                }
                else
                {
                    printf("Error : Unable to create file\n");
                }
            }
            else if(strcmp(command[0], "read") == 0)
            {
                fd = atoi(command[1]);
                ptr = (char *)malloc(sizeof(atoi(command[2])) + 1);

                if(ptr == NULL)
                {
                    printf("Memory allocation failed\n");
                    continue;
                }

                ret = ReadFile(fd, ptr, atoi(command[2]));

                if(ret > 0)
                {
                    write(1, ptr, ret);   // Print data
                    printf("\n");
                }
                else
                {
                    printf("Error : Unable to read file\n");
                }

                free(ptr);
            }
            else if(strcmp(command[0], "unlink") == 0)
            {
                ret = UnlinkFile(command[1]);

                if(ret == 0)
                {
                    printf("File deleted successfully\n");
                }
                else
                {
                    printf("Error : File not found\n");
                }
            }
            else
            {
                printf("Command not found\n");
            }
        }

        // WRITE COMMAND
        else if(count == 2)
        {
            if(strcmp(command[0], "write") == 0)
            {
                fd = atoi(command[1]);

                printf("Enter data to write:\n");
                fgets(arr, 1024, stdin);

                ret = WriteFile(fd, arr, strlen(arr));

                if(ret > 0)
                {
                    printf("Written successfully\n");
                }
                else
                {
                    printf("Error : Unable to write\n");
                }
            }
            else
            {
                printf("Command not found\n");
            }
        }

        else
        {
            printf("Command not found\n");
        }
    }

    return 0;
}