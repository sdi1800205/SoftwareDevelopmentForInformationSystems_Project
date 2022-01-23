#include <stdio.h>
#include <pthread.h>

#define MAX_INT 0x7FFFFFFF

// compares 2 pthreads_t
int compare_pthread_t(void* a, void* b) {
	if (*(pthread_t*)a > *(pthread_t*)b)
		return 1;
	else if (*(pthread_t*)a == *(pthread_t*)b)
		return 0;
	else
		return -1;
}

// hash function for pthread_t
unsigned int hash_pthread_t(void* value) {
    return *(pthread_t*)value%MAX_INT;
}

int main() {
    pthread_t a = 139850402563843;
    pthread_t b = 139850402563841;

    printf("hash value of a %u\n", hash_pthread_t(&a));
    printf("hash value of b %u\n", hash_pthread_t(&b));

    return 0;
}