#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "arena.h"

SharedData *data;

Weapon weapons[MAX_WEAPONS] = {
    {"Wood Sword", 100, 5},
    {"Iron Sword", 300, 15},
    {"Steel Axe", 600, 30},
    {"Demon Blade", 1500, 60},
    {"God Slayer", 5000, 150}
};

void header(){
    printf("\n==============================\n");
    printf("   ETERION BATTLE ARENA\n");
    printf("==============================\n");
}

int get_damage(Player p){
    int w = (p.weapon == -1) ? 0 : weapons[p.weapon].damage;
    return BASE_DAMAGE + (p.xp/50) + w;
}

void level_up(Player *p){
    int lvl = (p->xp/100)+1;
    if(lvl > p->level){
        p->level = lvl;
        printf("LEVEL UP -> %d\n", lvl);
    }
}

void add_history(int idx, char *opp, char *res, int xp){
    Player *p = &data->players[idx];
    if(p->history_count >= MAX_HISTORY) return;

    History *h = &p->history[p->history_count++];

    strcpy(h->opponent, opp);
    strcpy(h->result, res);
    h->xp_gain = xp;

    time_t t = time(NULL);
    strftime(h->time, 20, "%H:%M", localtime(&t));
}

void show_history(int idx){
    Player p = data->players[idx];

    printf("\n=== HISTORY ===\n");
    for(int i=p.history_count-1;i>=0;i--){
        printf("%s vs %s %s +%dXP\n",
            p.history[i].time,
            p.history[i].opponent,
            p.history[i].result,
            p.history[i].xp_gain);
    }
}

void show_profile(int idx){
    Player p = data->players[idx];

    printf("\n=== PROFILE ===\n");
    printf("Name: %s\n", p.username);
    printf("Lvl : %d\n", p.level);
    printf("Gold: %d\n", p.gold);
    printf("XP  : %d\n", p.xp);
}

void armory(int idx){
    Player *p = &data->players[idx];

    printf("\n=== ARMORY ===\n");
    printf("Gold: %d\n", p->gold);

    for(int i=0;i<MAX_WEAPONS;i++){
        printf("%d. %s (%dG +%d dmg)\n",
            i+1, weapons[i].name, weapons[i].price, weapons[i].damage);
    }

    int c; scanf("%d",&c);

    if(c>=1 && c<=MAX_WEAPONS){
        if(p->gold >= weapons[c-1].price){
            p->gold -= weapons[c-1].price;
            p->weapon = c-1;
            printf("Equipped!\n");
        } else printf("Not enough gold\n");
    }
}

void battle(int idx){
    printf("\nSearching opponent...\n");

    pthread_mutex_lock(&data->lock);

    if(data->waiting_player == -1){
        data->waiting_player = idx;
        pthread_mutex_unlock(&data->lock);

        int wait=0;
        while(wait<5){
            sleep(1);

            pthread_mutex_lock(&data->lock);
            if(data->opponent[idx]!=-1){
                pthread_mutex_unlock(&data->lock);
                break;
            }
            pthread_mutex_unlock(&data->lock);

            wait++;
        }

        pthread_mutex_lock(&data->lock);

        if(data->opponent[idx]==-1){
            data->waiting_player=-1;
            pthread_mutex_unlock(&data->lock);

            printf("Fight BOT\n");

            int hp=100;
            while(hp>0){
                hp-=get_damage(data->players[idx]);
                usleep(200000);
            }

            printf("WIN!\n");

            pthread_mutex_lock(&data->lock);
            data->players[idx].xp+=50;
            data->players[idx].gold+=120;
            level_up(&data->players[idx]);
            add_history(idx,"BOT","WIN",50);
            pthread_mutex_unlock(&data->lock);

            return;
        }

        pthread_mutex_unlock(&data->lock);
    }

    else{
        int enemy=data->waiting_player;

        data->opponent[idx]=enemy;
        data->opponent[enemy]=idx;
        data->waiting_player=-1;

        pthread_mutex_unlock(&data->lock);
    }

    while(1){
        pthread_mutex_lock(&data->lock);
        int enemy=data->opponent[idx];
        pthread_mutex_unlock(&data->lock);

        if(enemy!=-1) break;
        usleep(100000);
    }

    int enemy;

    pthread_mutex_lock(&data->lock);
    enemy=data->opponent[idx];
    pthread_mutex_unlock(&data->lock);

    printf("MATCH FOUND!\n");
    printf("%s vs %s\n",
        data->players[idx].username,
        data->players[enemy].username);

    int my_hp=BASE_HP;
    int enemy_hp=BASE_HP;

    while(my_hp>0 && enemy_hp>0){
        enemy_hp-=get_damage(data->players[idx]);
        if(enemy_hp<=0) break;

        my_hp-=get_damage(data->players[enemy]);

        printf("You:%d Enemy:%d\n",my_hp,enemy_hp);
        usleep(300000);
    }

    pthread_mutex_lock(&data->lock);

    if(my_hp>0){
        printf("WIN!\n");
        data->players[idx].xp+=50;
        data->players[idx].gold+=120;
        level_up(&data->players[idx]);
        add_history(idx,data->players[enemy].username,"WIN",50);
    } else{
        printf("LOSS\n");
        data->players[idx].xp+=15;
        data->players[idx].gold+=30;
        level_up(&data->players[idx]);
        add_history(idx,data->players[enemy].username,"LOSS",15);
    }

    data->opponent[idx]=-1;
    data->opponent[enemy]=-1;

    pthread_mutex_unlock(&data->lock);
}

void register_user(){
    char u[50],p[50];
    printf("Username: "); scanf("%s",u);
    printf("Password: "); scanf("%s",p);

    pthread_mutex_lock(&data->lock);

    for(int i=0;i<data->player_count;i++){
        if(strcmp(data->players[i].username,u)==0){
            printf("Already exist\n");
            pthread_mutex_unlock(&data->lock);
            return;
        }
    }

    Player *pl=&data->players[data->player_count++];

    strcpy(pl->username,u);
    strcpy(pl->password,p);
    pl->gold=150;
    pl->xp=0;
    pl->level=1;
    pl->active=0;
    pl->weapon=-1;
    pl->history_count=0;

    pthread_mutex_unlock(&data->lock);

    printf("Register success\n");
}

int login_user(){
    char u[50],p[50];
    printf("Username: "); scanf("%s",u);
    printf("Password: "); scanf("%s",p);

    pthread_mutex_lock(&data->lock);

    for(int i=0;i<data->player_count;i++){
        if(strcmp(data->players[i].username,u)==0 &&
           strcmp(data->players[i].password,p)==0){

            if(data->players[i].active){
                printf("Already login\n");
                pthread_mutex_unlock(&data->lock);
                return -1;
            }

            data->players[i].active=1;
            pthread_mutex_unlock(&data->lock);
            printf("Login success\n");
            return i;
        }
    }

    pthread_mutex_unlock(&data->lock);
    printf("Login failed\n");
    return -1;
}

int main(){
    key_t key=1234;
    int shmid=shmget(key,sizeof(SharedData),0666);

    if(shmid<0){
        printf("Orion belum jalan\n");
        return 0;
    }

    data=shmat(shmid,NULL,0);

    while(1){
        header();
        printf("1.Register\n2.Login\n3.Exit\nChoice:");

        int c; scanf("%d",&c);

        if(c==1) register_user();
        else if(c==2){
            int id=login_user();
            if(id==-1) continue;

            while(1){
                show_profile(id);

                printf("\n1.Battle\n2.Armory\n3.History\n4.Logout\nChoice:");
                scanf("%d",&c);

                if(c==1) battle(id);
                else if(c==2) armory(id);
                else if(c==3) show_history(id);
                else if(c==4){
                    pthread_mutex_lock(&data->lock);
                    data->players[id].active=0;
                    pthread_mutex_unlock(&data->lock);
                    break;
                }
            }
        }
        else break;
    }
}
