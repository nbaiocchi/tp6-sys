#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "bag.h"

bag_t * bb_create(int capacity)
{
    assert(capacity > 0);

    
    bag_t *bag=malloc(sizeof(bag_t));
    assert(bag != NULL);

    sem_init(&bag->semaphore, 0, 1);

    sem_init(&bag->semaphore2, 0, 0);

    sem_init(&bag->mutex, 0, 1);

    bag->elem = malloc( capacity * sizeof(void*));
    assert(bag->elem != NULL);

    bag->capacity  = capacity;
    bag->size = 0;         // 'size' == index of first free slot in 'elem'
    bag->is_closed = 0;

    return bag;
}

void bb_add(bag_t * bag, void * element)
{
    assert(bag != NULL);                 // sanity check

    sem_wait(&bag->semaphore);


    while( bag->size >= bag->capacity ) { } // CAUTION: this synchronization is bogus

    sem_wait(&bag->mutex);
    
    assert( bag-> is_closed == 0 );   // adding to a closed bag is an error
    assert( bag->size < bag->capacity ); // sanity check

    bag->elem[bag->size] = element;
    bag->size += 1;

    sem_post(&bag->mutex);

    sem_post(&bag->semaphore2);
}

void * bb_take(bag_t *bag)
{
    assert(bag != NULL); // sanity check

    sem_wait(&bag->semaphore2);

    while( bag->size <= 0 ) { } // CAUTION: this synchronization is bogus
    
    assert( bag->size > 0); // sanity check
    usleep(rand() % 10);// artificial delay to increase the occurence of race conditions

    bag->size -= 1;
    void *r = bag->elem[bag->size];
    bag->elem[bag->size] = NULL;

    sem_post(&bag->semaphore);
    
    return r;
}

void bb_close(bag_t *bag)
{
    assert("not implemented" == 0);
}
