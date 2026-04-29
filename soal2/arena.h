#ifndef ARENA_H
#define ARENA_H

#include <pthread.h>

#define MAX_PLAYERS 100
#define MAX_WEAPONS 5
#define MAX_HISTORY 50

#define BASE_DAMAGE 10
#define BASE_HP 100

typedef struct {
    char name[50];
    int price;
    int damage;
} Weapon;

typedef struct {
    char opponent[50];
    char result[10];
    int xp_gain;
    char time[20];
} History;

typedef struct {
    char username[50];
    char password[50];

    int gold;
    int level;
    int xp;

    int active;
    int weapon;

    History history[MAX_HISTORY];
    int history_count;

} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    int player_count;

    int waiting_player;
    int opponent[MAX_PLAYERS];

    pthread_mutex_t lock;
} SharedData;

#endif
