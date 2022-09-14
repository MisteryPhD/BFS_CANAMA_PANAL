#include <stdio.h>
#include <stdlib.h>

char *StepChange(char *array,int direction,int null_position)    /*this function will return char's array changed by moving space */
                                                                 /*symbol on the given direction */
{
    int i;
    char *buff=calloc(12,sizeof(char));    /*here memory for new array is allocated*/
    for(i=0;i<12;i++)    
    {
        buff[i]=array[i];    /*at first copy the array*/
    }
    switch (direction)    /*then move space*/
    {
        case 1:        /*up*/
            {
                buff[null_position]=array[null_position-6];
                buff[null_position-6]=array[null_position];
                break;
            }
        case 2:        /*down*/
            {
                buff[null_position]=array[null_position+6];
                buff[null_position+6]=array[null_position];
                break;
            }
        case 3:        /*left*/
            {
                buff[null_position]=array[null_position-1];
                buff[null_position-1]=array[null_position];
                break;
            }
        case 4:        /*right*/
            {
                buff[null_position]=array[null_position+1];
                buff[null_position+1]=array[null_position];
                break;
            }
    
    }
    return buff;
}

int StateCompare(char *state1,char *state2)    /*this function will compare two char's array*/
{
    int i;
    for(i=0;i<12;i++)
        if(state1[i]!=state2[i])
            return 0;
    return 1;
}

/*--------Here the HASH structures definition will be placed*/
typedef struct hash_element        /*This structure uses for one hash element*/
{
    char *state;    /*this variable will contain barley-break state (pointer to array of 12 chars)*/
    int null_position;    /*this var will contain the index of space symbol in this state (it uses for faster access)*/
    struct hash_element *father;    /*this variable will link to the foregoing lement (element that is father of this element)*/
    struct hash_element *next;      /*this variable will link to the next hash_element of the current hash bucket - element*/
                                    /*that have similar hash index but another state*/
}hash_element;
typedef struct     /*this structure will contain hash table bucket - hash table elements with similar hash index*/
{
    int current_size;    /*it will be amounts of elements i this bucket*/
    hash_element *he;    /*and pointer to the last added element to this bucket (it is the head of the linked list)*/
} hash_bucket;
typedef struct     /*this structure wil inplement the hash table*/
{
    int size;                     /*it will conatain hash table array size (amount of hash table buckets)*/
    int current_bucket_number;    /*it will contain the occupied hash table buckets amount*/
    int current_element_number;   /*it will contain the hash elements amount*/
    hash_bucket *hash_array;      /*it will contain pointer to the hash table buckets array. Each element of this array is bucket,*/ 
                                  /*that is the linked list of hash elements*/
}HashTable;

unsigned int hash_sum(char* key)    /*this method implements the hash sum evaluator for 12  characters array*/
{
    
    /*    unsigned int j,i,hash; */
    /*    for(i=0;i<12;i++)      */    /*hash clculates by sum of products of characters value to it's index in the array*/
    /*    {                      */
    /*        hash+=key[i]*i;    */
    /*    }                      */
    /*    return hash;           */
    
    
   int i, j;            /*here is the crc32 hash sum calculator and it is not used now*/
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (i!=12) {
      byte = key[i];            /*Get next byte*/
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) 
	  {    /*Do eight times*/
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
   
    
}
void InitHash(int size,HashTable *ht)    /*this fucntion will initialize hash table*/
{
    ht->hash_array=calloc(size, sizeof(hash_bucket));    /*    here the memory for hash table allocates */
    ht->size=size;                                       /*    updating of the hash table size  */
    ht->current_element_number=0;                        /*    initializes of the current number */ 
    ht->current_bucket_number=0;
}

hash_element *AddHash(HashTable *HT,hash_element *father,char *state ,int null_position)    /*this function implements addig of the*/
                                                                                            /*element to the hash table*/
{
    int hash_index;              /* this var will contain index in the hash table bucket array. New element wil be placed by this index*/
    hash_bucket *buff_bucket;    /* it is buffer for hash elements pointer. It will be used for */
    hash_element *buff_element;  /* it is buffer for pointer to new hash element that will be returned by the end of this function*/
    
    hash_index=hash_sum(state)%HT->size;    /*there index of the new element in the hash table array will be clalculated */
                                            /*by the state array hash sum    */

    buff_bucket=HT->hash_array+hash_index;        /* here the buffer initializes by pointer to hash bucket array element with*/
                                                  /* similar to calculated hash_index*/
    buff_element= buff_bucket->he;
    if(buff_bucket->current_size!=0) /*    here we will check if the hash table already contains such element (element with*/
                                     /*similar state character array)*/
    {
        while(buff_element!=NULL)    /*All elements of the corresponding bucket will be checked*/
        {
            
            if(StateCompare(buff_element->state,state))    /*    if some element of the bucket have the similar*/
                                                           /*    state than drop it */
            {
                free(state);
                return NULL;    /*    (it will not be placed on the table, and fucntion will return NULL as */
                                /*    the pointer to created element) */
            }
            
            buff_element=buff_element->next;    /*    here all elements from the correspond bucket will be listed*/
        }
            
    }
    else
        HT->current_bucket_number=HT->current_bucket_number+1;
    
                                                    /*    the first element of list now creating*/
    buff_element=calloc(1,sizeof(hash_element));    /*    allocating memory for new bucket element */
    buff_element->father=father;                    /*    set the father element pointer*/
    buff_element->next=buff_bucket->he;             /*    the old first element now is second in this list*/
    buff_element->state=state;
    buff_element->null_position=null_position;
    buff_bucket->he=buff_element;
    
    buff_bucket->current_size = buff_bucket->current_size+1;
    HT->current_element_number=HT->current_bucket_number+1;
    
    return buff_element;            /*return pointer to the created hash element (element of the bucket linked list)*/
    
    
}
/*------------HASH END    */

/*------------Here will be the CIRCULAR QUEUE structures definition placed*/

typedef struct    /*this structure defines the circular queue*/
{
    int size;     /*maximum number of elements*/
    int start;    /*index of oldest element*/
    int end;      /*index at which to write new element*/
    int max_used_size;
    hash_element  **elems;    /*array of hash elements*/
} CircularQueue;
 
void cqInit(CircularQueue *cq, int size)     /*this function will initialize*/
{
    cq->size = size + 1;    /*set correct size of queue */
    cq->start = 0;          /*it is clear queu so index of it's begining*/
    cq->end = 0;            /*and of it's end is zero*/
    cq->max_used_size = 0;
    cq->elems = calloc(cq->size, sizeof(hash_element*));    /*    allocating memory for queue*/
}
 
int cqIsEmpty(CircularQueue *cq)    /*this function will check if the queue is empty*/
{
    return cq->end == cq->start;
}
int cqIsFull(CircularQueue *cq) 
{
    return (cq->end + 1) % cq->size == cq->start;
}

void cqWrite(CircularQueue *cq, hash_element *elem)    /* Write an element, overwriting oldest element if buffer is full.*/ 
{
    cq->elems[cq->end] = elem;
   
    cq->end = (cq->end + 1) % cq->size;
   
    if (cq->end == cq->start)
        cq->start = (cq->start + 1) % cq->size;        /*if full some element will be  overwrited */
    
}
 

void cqRead(CircularQueue *cq, hash_element **elem)    /* it will read oldest element into the pointer elem*/
{
    int current_size;
    *elem = cq->elems[cq->start];
    cq->start = (cq->start + 1) % cq->size;
    
    current_size=( ( (cq->end - cq->start) >= 0) ? (cq->end - cq->start) : (cq->size+ cq->end - cq->start) );
    cq->max_used_size = cq->max_used_size > current_size ? (cq->max_used_size) : current_size ;
       
}
/*------------QUEUE END*/

/*------------Here  the STACK structures definition will be placed*/
typedef struct Stack    /*    it defines stack of the hash elements. Stack here is the linked list*/
{
    hash_element *element;
    struct Stack *next;
}Stack;

void StackPush(hash_element *element,Stack **Head)    /*    it will write new element to the stack*/
{
    Stack *New_Element=calloc(1,sizeof(Stack));       /*    allocating of memory for the new element*/
    New_Element->element=element;                     /*    filling of the new element's fields by given data*/
    New_Element->next=*Head;
    *Head=New_Element;                                /*write new element to the top of the stack (to the begining of the linked list)*/
}
hash_element *StackPop(Stack **Head)
{
    hash_element *buff=(*Head)->element;
    *Head=(*Head)->next;
    return buff;
}
/*------------STACK END*/

int main()
{
    int i,j,k;
    char *start_position,*goal_position;
    hash_element *buff,*buff2;
    
    HashTable HT;
    CircularQueue QE;
    Stack *S=NULL;
    
    InitHash(100003,&HT);    /*initialization of the hash table*/
    
    cqInit(&QE, 50000);      /*initialization of the queue*/
    
    
    start_position=(char*)calloc(12,sizeof(char));    /*this array defines starting state*/
    start_position[0]='C';    start_position[1]='A';    start_position[2]='N';    start_position[3]='A';    start_position[4]='M';    start_position[5]='A';
    start_position[6]='P';    start_position[7]='A';    start_position[8]='N';    start_position[9]='A';    start_position[10]='L'; start_position[11]=' ';
    
    goal_position=calloc(12,sizeof(char));            /*this array defines goal state*/
    goal_position[0]='P';    goal_position[1]='A';    goal_position[2]='N';    goal_position[3]='A';    goal_position[4]='M';    goal_position[5]='A';
    goal_position[6]='C';    goal_position[7]='A';    goal_position[8]='N';    goal_position[9]='A';    goal_position[10]='L';  goal_position[11]=' ';
    
    cqWrite(&QE,AddHash(&HT,NULL,start_position,11)); /*The first element of the queue is the start position*/
    
    while(!cqIsEmpty(&QE))
    {
        cqRead(&QE, &buff);    /*DEQUEUE , reading from the queue*/
        
        if(StateCompare(buff->state,goal_position))    /*if the readed hash table element is equal to goal */
                                                       /*terminate breadth first search */
            break;
            
        if(buff->null_position%6!=5) /*here we will make the possible moves and add the result to the hash table and queue*/
        {
            buff2=AddHash(&HT,buff,StepChange(buff->state,4,buff->null_position),buff->null_position+1);    
			    /*If it possible*/              
			    /*the current hash table element state will be chabged by space moving to the direction defined by StepChange function.*/
                /*Then this array will be given to the add hash function, wich will try to add it to the hash table*/
                /*if hash table already contains element with such state it will not put this element to the hash table and return NULL*/
                /*if add hash return element it will be written to the queue*/            
            if(buff2!=NULL)
                cqWrite(&QE,buff2);
            
        }
        if(buff->null_position%6!=0)
        {
            buff2=AddHash(&HT,buff,StepChange(buff->state,3,buff->null_position),buff->null_position-1);
            if(buff2!=NULL)
                cqWrite(&QE,buff2);
        }
        if(buff->null_position>5)
        {
            buff2=AddHash(&HT,buff,StepChange(buff->state,1,buff->null_position),buff->null_position-6);
            if(buff2!=NULL)
                cqWrite(&QE,buff2);
        }
        if(buff->null_position<6)
        {
            buff2=AddHash(&HT,buff,StepChange(buff->state,2,buff->null_position),buff->null_position+6);
            if(buff2!=NULL)
                cqWrite(&QE,buff2);
        }
    }
    printf("Number of items in the hash table at the end is : %d\n",HT.current_element_number);
    printf("Number of empty hash table buckets is : %d\n",HT.size-HT.current_bucket_number);
    
    k=0;
    for(i=0;i<HT.size;i++)    /*here the number of buckets with only one lement counted*/
    {
        if(HT.hash_array[i].current_size==1)
            k++;
    }
    printf("Number of hash table buckets with only one item in them at the end is : %d\n",k);
    
    k=0;
    for(i=0;i<HT.size;i++)    /*here largest bucket is found*/
    {
        k=(HT.hash_array[i].current_size>k) ? HT.hash_array[i].current_size : k;
    }
    printf("Maximum number of items in a largest hash table  bucketat the end is : %d\n",k);
    
    printf("Number of positions in the queue at the end is : %d\n",( ( (QE.end - QE.start) >= 0) ? (QE.end - QE.start) : (QE.size+ QE.end - QE.start) ));
    printf("Maximum number of positions ever in the queue is : %d\n",QE.max_used_size);
    
    while(buff!=NULL)    /*here list of hash elements is pushed into stack (list from the goal to the starting position)*/
    {
        StackPush(buff,&S);
        buff=buff->father;
    }
    
    k=0;
    buff2=NULL;    /*here this var will contained one step before buff position*/
    
    while(S!=NULL)    /*while stack is not free*/
    {
        buff=StackPop(&S);         /*take element from the stack*/
        printf("\nStep %d ",k);    /*and starts to output information about current move*/
        if(k>0)    /*starts from the 1 step the information about move may be outputed*/
        {
            
            switch(buff->null_position - buff2->null_position)
            {
                case 1:
                {
                    printf(",move %c east",buff->state[buff2->null_position]);
                    break;
                }
                case -1:
                {
                    printf(",move %c west",buff->state[buff2->null_position]);
                    break;
                }
                case 6:
                {
                    printf(",move %c south",buff->state[buff2->null_position]);
                    break;
                }
                case -6:
                {
                    printf(",move %c north",buff->state[buff2->null_position]);
                    break;
                }
            }
            
        }
        
        printf(":\n\n");
        for(i=0;i<2;i++)    /*here just position is outputed*/
            {
                for(j=0;j<6;j++)
                printf("%c ",buff->state[i*6+j]);
                printf("\n");
            }
        k++;
        buff2=buff;
    }
        
    return 0;
    
    
}
