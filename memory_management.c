// Created by Archie Gerry for OS CWK1 on 25/11/22
// All works is original and complies with university AI policy

#include "memory_management.h"
#include "kernel/types.h"
#include "user/user.h"
#include <stddef.h>

struct Node
{
  int flag;                     //Key: 1 indicates allocated, 0 indicates unallocated, anything else is easter egg 
  unsigned long size;           //Size of this node (not including header)       
  struct Node* next;            //Pointer to the next node in the linked list
};

struct Node* heapStart;         //Start address for the linked list

//Memory allocation 
void* _malloc(int size)
{
  
  //if size is zero return NULL
  if (size == 0)
  {
    return NULL;
  }

  //Make a counting node and cast it to the heap start node
  struct Node* currentNode = heapStart;

  //To find the closest size chunk if there isn't an exact match
  struct Node* closestSize = NULL;
  unsigned long differenceCount, smallestDifference = 0;

  //loop forever
  while (1)
  {
    //if there has been no heap allocated then give it the space

    if (currentNode == NULL)
    {
      currentNode = (void*) (long) sbrk(sizeof(struct Node) + (unsigned long)size);
      currentNode->flag = 1;
      currentNode->size = (unsigned long)size;
      currentNode->next = NULL;
      heapStart = currentNode;
      return (void*)currentNode + sizeof(struct Node);
    }

    //if the space is unallocated and the size is of the requested length give it to user
    if (currentNode->flag == 0 && currentNode->size == (unsigned long)size)
    {
      currentNode->flag = 1;
      return  (void*)currentNode + sizeof(struct Node);
    }

    //The current node must have enough size to fit the user specified size, as well as leftover space for a new 
    // header and user memory space
    unsigned long minimumRequiredSize = (unsigned long)size + sizeof(struct Node);

    //If the current node is unallocated and has more than the minimum required size
    if (minimumRequiredSize < currentNode->size && currentNode->flag == 0)
    {
      differenceCount = currentNode->size - (unsigned long) size;

      //If this is the first time calculating the difference we need to update the total to be the first difference
      //The difference should never be 0 naturally as this function is preceded by a function to check if the sizes match (0 difference) 
      if (smallestDifference == 0)
      {
        smallestDifference = differenceCount;
        closestSize = currentNode;
      }

      //If the size difference is smaller than the smallest difference
      if (differenceCount < smallestDifference)
      {
        smallestDifference = differenceCount;
        closestSize = currentNode;  
      }
    }
    

    //if the node is the last link in the linked list, and mem has not been allocated
    if (currentNode->next == NULL)
    {
      //This must mean there is some larger free space we can use to allocate the memory to
      if (smallestDifference != 0)
      {
        struct Node* copyNode = heapStart;
        
        while (1)
        {
          if (copyNode == closestSize)
          {
            struct Node* remainderNode = copyNode + sizeof(struct Node) + (unsigned long)size;
            
            remainderNode->flag = 0;
            remainderNode->size = copyNode->size - sizeof(struct Node) - (unsigned long)size;
            remainderNode->next = copyNode->next;

            copyNode->flag = 1;
            copyNode->size = (unsigned long)size;
            copyNode->next = remainderNode;

            return  (void*)copyNode + sizeof(struct Node);
          }
          copyNode = copyNode->next;
        }
      }
    
     //If no larger free space then we must increase the heap size
     struct Node* tempNode = (void*) (long) sbrk(sizeof(struct Node) + (unsigned long)size);
     tempNode->flag = 1;
     tempNode->size = (unsigned long)size;
     currentNode->next = tempNode;
     return  (void*)tempNode + sizeof(struct Node); 
    }
  
    //Go to the next element in the linked list
    currentNode = currentNode->next;
  }
}

//Memory freeing 
void _free(void *ptr)
{
  //We need a counting node
  struct Node* currentNode = heapStart;

  //Loop forever
  while (1)
  {
    //ptr only points to the start of user data, we need the start of the chunk
    struct Node* nodePosition = (struct Node*) ptr - 1;

    //No heap allocated
    if (currentNode == NULL)
    {
      break;
    }
   
    //if the current chunk is the given chunk
    if (nodePosition == currentNode)
    {
      //change the fag to be unallocated
      currentNode->flag = 0;
      break;
    }

    //Cycle through the linked list
    currentNode = currentNode->next; 

    //If we are at the end of the linked list
    if (currentNode->next == NULL)
    {
      break;
    }  
  }
}

/*int main(void)
{
 
  void * test2 = _malloc(100);
  printf("%p\n", test2);


  void * test3 = _malloc(100);
  printf("%p\n", test3);
     


  void * test4 = _malloc(100);
  printf("%p\n", test4);


  
  void * test5 = _malloc(100);
  printf("%p\n", test5);


  
  _free(test4);
  
  void * test6 = _malloc(4);
  printf("%p\n", test6);



  return 0;
}*/


