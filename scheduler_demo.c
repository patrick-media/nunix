#include<stdio.h>
#include<stdlib.h>
#define NUM_PROCESSES 3
#define TIME_QUANTUM 3
typedef struct {
    int pid;
    int burst_time;
} process_t;
void proc_run( process_t* process ) {
    printf( "Running process %d for 1 unit of time.\n", process->pid );
    process->burst_time -= 1;
}
int main( void ) {
    process_t processes[ NUM_PROCESSES ];
    int current = 0;
    for( int i = 0; i < NUM_PROCESSES; i++ ) {
        processes[ i ].pid = i;
        processes[ i ].burst_time = rand() % 10 + 1;
        printf( "Process %d has a time of %d.\n", processes[ i ].pid, processes[ i ].burst_time );
    }
    while( 1 ) {
        if( processes[ current ].burst_time > 0 ) {
            proc_run( &processes[ current ] );
        }
        current = ( current + 1 ) % NUM_PROCESSES;
        int alldone = 1;
        for( int i = 0; i < NUM_PROCESSES; i++ ) {
            if( processes[ i ].burst_time > 0 ) {
                alldone = 0;
                break;
            }
        }
        if( alldone ) break;
    }
    printf( "All processes have completed.\n" );
    return 0;
}
