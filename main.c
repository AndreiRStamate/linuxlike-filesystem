#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

void SplitString(char* line, char*** words)
{
    char* buffer;
    int wordCount;
    int maxWords = 100;
    buffer = (char*) malloc(strlen(line) * sizeof(char));
    strcpy(buffer,line);

    (*words) = (char**) malloc(maxWords * sizeof(char**));

    wordCount = 0;

    (*words)[wordCount++] = strtok(buffer, " ");
    while (((*words)[wordCount++] = strtok(NULL, " ")) != NULL &&
            wordCount <= maxWords)
    {
    }
}

Directory* CreateFileSystem()
{
    Directory* rootDir = (Directory*)malloc(sizeof(Directory));

    rootDir->name = "/";

    rootDir->headDirectory = NULL;
    rootDir->headFile = NULL;
    rootDir->parentDir = NULL;

    return rootDir;
}

void DeleteFileSystem(Directory* rootDir, char** commands)
{
    free(rootDir->headDirectory);
    free(rootDir->headFile);
    free(rootDir);
    free(commands[0]);
    free(commands);
    return;
}

void PrintFileNode(void* n)
{
    printf("%s", ((File*)n)->name);
}

void PrintDirectoryNode(void* d)
{
    printf("%s", ((Directory*)d)->name);
}

int cmpFile(void* n1, void* n2)
{
    return strcmp(((File*)n1)->name, ((File*)n2)->name);
}

int cmpDirectory(void* n1, void* n2)
{
    return strcmp(((Directory*)n1)->name, ((Directory*)n2)->name);
}

void InsertFile(Node** headRef, void* data, size_t dataSize)
{
    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->data = malloc(dataSize);

    int i;
    for (i = 0; i < dataSize; i++)
    {
        *(char*)(newNode->data+i) = *(char*)(data+i);
    }

    if (*headRef == NULL || cmpFile((*headRef)->data, newNode->data) > 0)
    {
        newNode->next = (*headRef);
        (*headRef) = newNode;
    }
    else
    {
        Node* currentNode = *headRef;
        while (currentNode->next != NULL &&
               cmpFile(currentNode->next->data, newNode->data) < 0)
        {
            currentNode = currentNode->next;
        }

        newNode->next = currentNode->next;
        currentNode->next = newNode;
    }
}

void InsertDirectory(Node** headRef, void* data, size_t dataSize)
{
    Node* newNode = (Node*)malloc(sizeof(Node));

    newNode->data = malloc(dataSize);

    int i;
    for (i = 0; i < dataSize; i++)
    {
        *(char*)(newNode->data+i) = *(char*)(data+i);
    }

    if (*headRef == NULL || cmpDirectory((*headRef)->data, newNode->data) > 0)
    {
        newNode->next = (*headRef);
        (*headRef) = newNode;
    }
    else
    {
        Node* currentNode = *headRef;
        while (currentNode->next != NULL &&
               cmpDirectory(currentNode->next->data, newNode->data) < 0)
        {
            currentNode = currentNode->next;
        }

        newNode->next = currentNode->next;
        currentNode->next = newNode;
    }
}

void PrintList(Node *node, void (*fptr)(void *))
{
    while (node != NULL)
    {
        (*fptr)(node->data);
        if (node->next != NULL)
        {
            printf(" ");
        }
        node = node->next;
    }
}

void AddFile(Directory* dir, char* data, int size, char* name)
{
    File* newFile = (File*)malloc(sizeof(File));

    newFile->data = (char*)malloc(strlen(data)*sizeof(char));
    strcpy(newFile->data, data);

    newFile->size = size;

    newFile->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(newFile->name, name);

    newFile->dir = dir;


    InsertFile(&(dir->headFile), newFile, sizeof(File));
}

void AddDirectory(Directory* dir, char* name)
{
    Directory* newDirectory = (Directory*)malloc(sizeof(Directory));

    newDirectory->name = (char*)malloc(strlen(name)*sizeof(char));
    strcpy(newDirectory->name, name);

    newDirectory->headDirectory = NULL;

    newDirectory->headFile = NULL;

    newDirectory->parentDir = dir;

    InsertDirectory(&(dir->headDirectory), newDirectory, sizeof(Directory));
}

void ListFiles(Directory* dir)
{
    PrintList(dir->headFile, PrintFileNode);
    if (dir->headFile != NULL && dir->headDirectory != NULL)
    {
        printf(" ");
    }
    PrintList(dir->headDirectory, PrintDirectoryNode);
    printf("\n");
}

void PrintWorkingDirectory(Directory* dir)
{
    Directory* tempDir = dir;
    char dirNames[100][100];

    int i = 0;
    while (tempDir->parentDir != NULL)
    {
        strcpy(dirNames[i], tempDir->name);
        tempDir = tempDir->parentDir;
        i++;
    }
    strcpy(dirNames[i], "/");
    if (i == 0)
    {
        printf("/\n");
        return;
    }
    i--;
    printf("/");
    while (i >= 0)
    {
        printf("%s", dirNames[i]);
        i--;
        if (i >= 0)
        {
            printf ("/");
        }
    }
    printf("\n");
}

void TabHelper(int i)
{
    while (i--)
    {
        printf("    ");
    }
}

void SearchFileList(Directory* dir, int i)
{
    Node* tempNode = dir->headFile;

    while (tempNode != NULL)
    {
        TabHelper(i);
        PrintFileNode(tempNode->data);
        printf("\n");
        tempNode = tempNode->next;
    }
}

void SearchDirectoryList(Directory* dir, int i)
{
    Node* tempNode = dir->headDirectory;
    while (tempNode != NULL)
    {
        TabHelper(i);
        PrintDirectoryNode(tempNode->data);
        printf("\n");

        SearchFileList(((Directory*)tempNode->data), i+1);

        SearchDirectoryList(tempNode->data, i+1);

        tempNode = tempNode->next;
    }
}

void Tree(Directory* dir)
{
    int i = 1;
    printf("%s\n", dir->name);
    SearchFileList(dir, i);
    SearchDirectoryList(dir, i);
}

void RemoveFile(Node** headRef, File* file)
{
    Node* tempNode = *headRef;
    Node* prevNode;

    if (tempNode != NULL &&
        strcmp( ((File*)(tempNode->data))->name , file->name) == 0)
        {
            *headRef = tempNode->next;
            free(tempNode);
            return;
        }
    while (tempNode != NULL &&
           strcmp( ((File*)(tempNode->data))->name , file->name) != 0)
            {
                prevNode = tempNode;
                tempNode = tempNode->next;
            }
    if (tempNode == NULL)
    {
        return;
    }
    prevNode->next = tempNode->next;
    free(tempNode);
}

void RemoveFileList(Node** headRef)
{
    while (*headRef != NULL)
    {
        Node* tempNode = (*headRef);
        (*headRef) = tempNode->next;
        free(tempNode);
    }
}

void RemoveDirectory(Node** headRef, Directory* dir)
{
    Node* tempNode = *headRef;
    Node* prevNode;

    if (tempNode != NULL &&
        strcmp( ((Directory*)(tempNode->data))->name , dir->name) == 0)
        {
            *headRef = tempNode->next;
            free(tempNode);
            return;
        }
    while (tempNode != NULL &&
           strcmp( ((Directory*)(tempNode->data))->name , dir->name) != 0)
            {
                prevNode = tempNode;
                tempNode = tempNode->next;
            }
    if (tempNode == NULL)
    {
        return;
    }
    prevNode->next = tempNode->next;
    free(tempNode);
}

void RemoveDirectoryList(Node** headRef)
{
    {
        Node* tempNode = *headRef;
        while (tempNode != NULL)
        {
            RemoveFileList(&(*(Directory*)tempNode->data).headFile);
            RemoveDirectoryList(&(*(Directory*)tempNode->data).headDirectory);
            tempNode = tempNode->next;
        }
    }
    while (*headRef != NULL)
    {
        Node* tempNode = (*headRef);
        (*headRef) = tempNode->next;
        free(tempNode);
    }
}

int main()
{
    int shouldRun = 1;

    char commandBuffer[100];
    char** command;

    Directory* rootDir = NULL;
    Directory* currentDir = NULL;
    while(shouldRun)
    {
        gets(commandBuffer);
        SplitString(commandBuffer, &command);

        if (strcmp(command[0], "create") == 0)
        {
            if (strcmp(command[1], "fs") == 0)
            {
                rootDir = CreateFileSystem();
                currentDir = rootDir;
            }
        }
        else if (strcmp(command[0], "delete") == 0)
        {
            if (strcmp(command[1], "fs") == 0)
            {
                DeleteFileSystem(rootDir, command);
                shouldRun = 0;
            }
        }
        else if (strcmp(command[0], "touch") == 0)
        {
            AddFile(currentDir, command[2], strlen(command[2]), command[1]);
        }
        else if (strcmp(command[0], "mkdir") == 0)
        {
            AddDirectory(currentDir, command[1]);
        }
        else if (strcmp(command[0], "ls") == 0)
        {
            ListFiles(currentDir);
        }
        else if (strcmp(command[0], "pwd") == 0)
        {
            PrintWorkingDirectory(currentDir);
        }
        else if (strcmp(command[0], "cd") == 0)
        {
            if (strcmp(command[1], "..") == 0)
            {
                currentDir = currentDir->parentDir;
            }
            else if (strcmp(command[1], ".") == 0)
            {
                printf("\n%s\n%s\n", currentDir->name, currentDir->parentDir->name);
            }
            else
            {
                int foundDir = 0;
                Node* tempNode = currentDir->headDirectory;
                while (tempNode != NULL && !foundDir)
                {
                    Directory* auxData = tempNode->data;

                    if (strcmp(auxData->name,command[1]) == 0)
                    {
                        foundDir = 1;
                        currentDir = auxData;
                    }
                    else
                    {
                        tempNode = tempNode->next;
                    }
                }
                if (!foundDir)
                {
                    printf("Cannot move to '%s': No such directory!\n", command[1]);
                }
            }
        }
        else if (strcmp(command[0], "tree") == 0)
        {
            Tree(currentDir);
        }
        else if (strcmp(command[0], "rm") == 0)
        {
            int foundFile = 0;
            Node* tempNode = currentDir->headFile;
            while (tempNode != NULL && !foundFile)
            {
                File* auxData = tempNode->data;
                if (strcmp(auxData->name, command[1]) == 0)
                {
                    foundFile = 1;
                    RemoveFile(&currentDir->headFile, auxData);
                }
                else
                {
                    tempNode = tempNode->next;
                }
            }
            if (!foundFile)
            {
                printf("Cannot remove '%s': No such file!\n", command[1]);
            }
        }
        else if (strcmp(command[0], "rmdir") == 0)
        {
            int foundDirectory = 0;
            Node* tempNode = currentDir->headDirectory;
                while (tempNode != NULL && !foundDirectory)
                {
                    Directory* auxData = tempNode->data;

                    if (strcmp(auxData->name,command[1]) == 0)
                    {
                        foundDirectory = 1;
                        RemoveFileList(&auxData->headFile);
                        RemoveDirectoryList(&auxData->headDirectory);
                        RemoveDirectory(&currentDir->headDirectory, auxData);
                    }
                    else
                    {
                        tempNode = tempNode->next;
                    }
                }
                if (!foundDirectory)
                {
                    printf("Cannot remove '%s': No such directory!\n", command[1]);
                }
        }
    }

    return 0;
}
