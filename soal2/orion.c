#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "arena.h"

int shmid;
SharedData *data;

void cleanup(int sig){
    shmctl(shmid, IPC_RMID, NULL);
    printf("\n[System] Shared memory cleaned.\n");
    exit(0);
}

int main(){
    signal(SIGINT, cleanup);

    key_t key = 1234;
    shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);

    data = shmat(shmid, NULL, 0);

    data->player_count = 0;
    data->waiting_player = -1;

    pthread_mutex_init(&data->lock, NULL);

    for(int i=0;i<MAX_PLAYERS;i++){
        data->players[i].active = 0;
        data->opponent[i] = -1;
    }

    printf("Orion is ready (PID: %d)\n", getpid());

    while(1) sleep(1);
}
