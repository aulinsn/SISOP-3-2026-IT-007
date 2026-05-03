# Laporan Resmi Praktikum Sistem Operasi Modul 3

---

## Identitas

| Nama                    | NRP        |
| ----------------------- | ---------- |
| Arjunina Maqbulin Usman | 5027251007 |

---

# Soal 1 : Present Day, Preseny

Program ini dibuat untuk:

* Membangun sistem komunikasi berbasis client-server menggunakan socket TCP
* Menangani banyak client secara bersamaan menggunakan thread
* Mengirim pesan secara broadcast ke seluruh client
* Menyediakan sistem admin (The Knights)
* Mencatat seluruh aktivitas ke dalam file log

---

## Alur Program:

1. Server dijalankan dan menunggu koneksi
2. Client terhubung ke server dan mengirim username
3. Server membuat thread untuk setiap client
4. Client dapat mengirim pesan
5. Server menerima dan melakukan broadcast
6. Admin dapat menjalankan command khusus
7. Semua aktivitas dicatat dalam log

---

## Penjelasan Kode

### 1. Membuat Socket Server (`wired.c`)

```c
server_fd = socket(AF_INET, SOCK_STREAM, 0);
```

#### Penjelasan:

* Membuat socket menggunakan TCP
* `AF_INET` untuk IPv4
* `SOCK_STREAM` untuk komunikasi berbasis koneksi

---

### 2. Binding dan Listen

```c
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(PORT);

bind(server_fd, (struct sockaddr*)&address, sizeof(address));
listen(server_fd, 10);
```

#### Penjelasan:

* Menghubungkan socket ke port tertentu
* Server siap menerima koneksi client

---

### 3. Accept Client dan Thread

```c
int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

int *p = malloc(sizeof(int));
*p = new_socket;

pthread_create(&tid, NULL, handle_client, p);
pthread_detach(tid);
```

#### Penjelasan:

* `accept()` menerima client baru
* Setiap client ditangani oleh thread (`pthread`)
* `pthread_detach()` agar thread berjalan independen

---

### 4. Menerima Username

```c
read(sock, name, sizeof(name));
name[strcspn(name, "\n")] = 0;
```

#### Penjelasan:

* Server membaca nama client
* `strcspn` digunakan untuk menghapus newline

---

### 5. Menyimpan Client

```c
clients[client_count] = sock;
strcpy(client_names[client_count], name);
client_count++;
```

#### Penjelasan:

* Menyimpan socket dan nama client
* Digunakan untuk broadcast dan admin

---

### 6. Broadcast Pesan

```c
void broadcast(char *msg, int sender) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i] != sender) {
            send(clients[i], msg, strlen(msg), 0);
        }
    }
}
```

#### Penjelasan:

* Mengirim pesan ke semua client kecuali pengirim
* merupakan inti dari sistem komunikasi

---

### 7. Sinkronisasi (Mutex)

```c
pthread_mutex_lock(&lock);
// akses data client
pthread_mutex_unlock(&lock);
```

#### Penjelasan:

* Digunakan saat:

  * menambah client
  * menghapus client
  * broadcast
* mencegah race condition

---

### 8. Logging

```c
FILE *f = fopen("history.log", "a");
fprintf(f, "[timestamp] [type] message\n");
fclose(f);
```

#### Penjelasan:

* Semua aktivitas dicatat ke `history.log`
* termasuk:

  * user connect
  * disconnect
  * chat
  * admin command

---

### 9. Sistem Admin (The Knights)

```c
if (strcmp(name, "The Knights") == 0)
```

#### Penjelasan:

* Jika username "The Knights", maka masuk mode admin
* Admin diminta password:

```c
if (strcmp(password, "protocol7") == 0)
```

---

### 10. Command Admin

```c
if (cmd[0] == '1') // Active Users
if (cmd[0] == '2') // Uptime
if (cmd[0] == '3') // Shutdown
```

#### Penjelasan:

* **1 → Active Users**

  * menampilkan semua client yang aktif
* **2 → Uptime**

  * menghitung waktu server berjalan
* **3 → Shutdown**

  * menutup semua client
  * menghentikan server

---

## Client (`navi.c`)

### 1. Connect ke Server

```c
sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
```

#### Penjelasan:

* Client terhubung ke server menggunakan TCP

---

### 2. Mengirim Username

```c
send(sock, name, strlen(name), 0);
```

---

### 3. Mode Admin

```c
if (strcmp(name, "The Knights") == 0)
```

#### Penjelasan:

* Jika admin:

  * input password
  * menjalankan command

---

### 4. Thread untuk Menerima Pesan

```c
pthread_create(&recv_thread, NULL, receive_msg, NULL);
```

#### Penjelasan:

* Digunakan agar client bisa:

  * menerima pesan
  * dan tetap bisa mengetik

---

### 5. Mengirim Pesan

```c
send(sock, message, strlen(message), 0);
```

---

### 6. Menerima Pesan

```c
read(sock, buffer, sizeof(buffer));
printf("%s", buffer);
```

#### Penjelasan:

* Client menerima broadcast dari server

---

### 7. Keluar dari Program

```c
if (strcmp(message, "exit") == 0) break;
```

#### Penjelasan:

* Client keluar jika mengetik "exit"
* socket ditutup dengan `close(sock)`


### Output
1. Setelah program berhasil di compile dan di jalankan       
   <img width="468" height="136" alt="image" src="https://github.com/user-attachments/assets/f66e8422-db38-48a9-b4f5-b79b33cc549d" />      
   <img width="461" height="60" alt="image" src="https://github.com/user-attachments/assets/7db47898-bc60-4722-ba65-08af22f71e6e" />        
   <img width="457" height="85" alt="image" src="https://github.com/user-attachments/assets/ad31ef30-ad20-4077-b4a8-a5408c923a7e" />       

2. Setelah menjalankan client ke-2       
   <img width="460" height="62" alt="image" src="https://github.com/user-attachments/assets/73f2ebad-29d5-43f8-8bee-99db32a45ab6" />       
   <img width="461" height="75" alt="image" src="https://github.com/user-attachments/assets/e09a7ac6-56df-4d6b-92d3-95a67c1fa7e7" />       
   <img width="454" height="105" alt="image" src="https://github.com/user-attachments/assets/056f4fb2-2383-4ebc-8c1a-4e7f39538393" />       

3. Saat client exit       
   <img width="462" height="104" alt="image" src="https://github.com/user-attachments/assets/7d6de303-6559-4a48-b449-2f8b24ef6001" />       
   <img width="452" height="127" alt="image" src="https://github.com/user-attachments/assets/6c7aa465-0ca3-4c12-88e5-003eab21af3f" />      

4. Menjalankan mode admin       
   <img width="452" height="127" alt="image" src="https://github.com/user-attachments/assets/cd73ec49-17e5-48f7-a490-dada61299930" />             
   Active users:       
   <img width="463" height="217" alt="image" src="https://github.com/user-attachments/assets/8ec29c9b-892c-4abe-9b89-61c48a458b08" />          
   Uptime:
   <img width="274" height="121" alt="image" src="https://github.com/user-attachments/assets/8cab2811-8c27-44da-a9a9-e671681b17e7" />
   Shutdown:
   <img width="464" height="140" alt="image" src="https://github.com/user-attachments/assets/b4a1b8d1-7836-4ae9-a37c-e900460717e5" />
   <img width="461" height="159" alt="image" src="https://github.com/user-attachments/assets/12a95445-2a76-4327-9602-14d4a9000f8d" />
   <img width="464" height="116" alt="image" src="https://github.com/user-attachments/assets/54af7655-7800-4ae2-a9f6-b51ee448a86f" />
   Exit:
   <img width="466" height="130" alt="image" src="https://github.com/user-attachments/assets/4bad3813-36ca-4154-8943-ee12ca73a292" />
   <img width="469" height="72" alt="image" src="https://github.com/user-attachments/assets/8b945f42-6fa7-4100-bb5a-2354ece6b3a9" />        

5. Isi `history.log`      
   <img width="468" height="295" alt="image" src="https://github.com/user-attachments/assets/d531184d-8c71-45b1-a4db-ac50e7e97944" />





## Soal 2 : The Battle of Eterion (Multi-Process Simulation)

Program ini dibuat untuk:

* Mengimplementasikan komunikasi antar proses menggunakan **shared memory**
* Mengelola sistem game berbasis multi-process
* Menyinkronkan akses data menggunakan mutex
* Menghindari race condition pada data bersama

---

## Alur Program:

1. Program `orion` membuat shared memory
2. Program `eternal` terhubung ke shared memory
3. User melakukan register / login
4. Player masuk ke sistem arena
5. Sistem melakukan matchmaking
6. Battle berlangsung (player vs player / bot)
7. Data player diperbarui
8. Reward dan history disimpan
9. Program berjalan berulang

---

## Penjelasan

---

### 1. Struktur Shared Memory (`arena.h`)

```c
typedef struct {
    Player players[MAX_PLAYERS];
    int player_count;

    int waiting_player;
    int opponent[MAX_PLAYERS];

    pthread_mutex_t lock;
} SharedData;
```

#### Penjelasan:

* Menyimpan seluruh data player
* `waiting_player` → player yang sedang menunggu lawan
* `opponent[]` → pasangan battle
* `lock` → untuk sinkronisasi akses data



---

### 2. Membuat Shared Memory (`orion.c`)

```c
key_t key = 1234;
shmid = shmget(key, sizeof(SharedData), IPC_CREAT | 0666);
data = shmat(shmid, NULL, 0);
```

#### Penjelasan:

* `shmget()` membuat shared memory
* `shmat()` menghubungkan memory ke program
* digunakan sebagai media komunikasi antar proses



---

### 3. Inisialisasi Data

```c
data->player_count = 0;
data->waiting_player = -1;

pthread_mutex_init(&data->lock, NULL);
```

#### Penjelasan:

* Mengatur kondisi awal sistem
* `waiting_player = -1` → belum ada yang menunggu
* mutex digunakan untuk menjaga konsistensi data

---

### 4. Cleanup Shared Memory

```c
void cleanup(int sig){
    shmctl(shmid, IPC_RMID, NULL);
}
```

#### Penjelasan:

* Menghapus shared memory saat program dihentikan
* mencegah memory leak

---

### 5. Mengakses Shared Memory (`eternal.c`)

```c
key_t key=1234;
int shmid=shmget(key,sizeof(SharedData),0666);
data=shmat(shmid,NULL,0);
```

#### Penjelasan:

* `eternal` hanya mengakses shared memory yang sudah dibuat
* jika belum ada → program tidak berjalan



---

### 6. Sistem Register

```c
Player *pl=&data->players[data->player_count++];

strcpy(pl->username,u);
strcpy(pl->password,p);
pl->gold=150;
pl->xp=0;
pl->level=1;
```

#### Penjelasan:

* Menambahkan player baru ke shared memory
* Set atribut awal:

  * gold
  * XP
  * level

---

### 7. Sistem Login

```c
if(strcmp(data->players[i].username,u)==0 &&
   strcmp(data->players[i].password,p)==0){
```

#### Penjelasan:

* Validasi username dan password
* memastikan user tidak login ganda

---

### 8. Matchmaking System

```c
if(data->waiting_player == -1){
    data->waiting_player = idx;
}
```

#### Penjelasan:

* Jika belum ada player:

  * masuk ke antrian
* Jika ada:

  * langsung dipasangkan

---

### 9. Battle System

```c
enemy_hp -= get_damage(data->players[idx]);
my_hp -= get_damage(data->players[enemy]);
```

#### Penjelasan:

* Pertarungan berlangsung real-time
* damage dihitung dari:

  * base damage
  * XP
  * weapon

---

### 10. Melawan Bot

```c
if(data->opponent[idx]==-1){
    printf("Fight BOT\n");
}
```

#### Penjelasan:

* Jika tidak ada player lain
* sistem otomatis melawan bot

---

### 11. Reward System

```c
data->players[idx].xp += 50;
data->players[idx].gold += 120;
```

#### Penjelasan:

* Player mendapatkan reward setelah menang
* XP digunakan untuk leveling

---

### 12. Level Up

```c
int lvl = (p->xp/100)+1;
```

#### Penjelasan:

* Level meningkat berdasarkan XP
* sistem progression player

---

### 13. Armory System

```c
if(p->gold >= weapons[c-1].price){
    p->weapon = c-1;
}
```

#### Penjelasan:

* Player dapat membeli senjata
* meningkatkan damage

---

### 14. History System

```c
strcpy(h->opponent, opp);
strcpy(h->result, res);
```

#### Penjelasan:

* Menyimpan riwayat battle
* berisi:

  * lawan
  * hasil
  * XP

---

### 15. Sinkronisasi (Mutex)

```c
pthread_mutex_lock(&data->lock);
// akses shared memory
pthread_mutex_unlock(&data->lock);
```

#### Penjelasan:

* Menghindari race condition
* memastikan data konsisten saat diakses banyak proses

---

#### Output
1. Saat compile dan run orion
   <img width="463" height="120" alt="image" src="https://github.com/user-attachments/assets/3e79a249-6554-4bc1-8066-54b466cefe7c" />

2. Saat run eternal
   <img width="465" height="150" alt="image" src="https://github.com/user-attachments/assets/92afad04-2061-4f57-9479-9a3e01bcf686" />

3. Saat user sudah berhasil register dan login
   <img width="231" height="151" alt="image" src="https://github.com/user-attachments/assets/7c446cc3-dd93-4089-b6b7-228d85acbf0b" />
   <img width="210" height="291" alt="image" src="https://github.com/user-attachments/assets/0f3fd83d-fc14-4b84-a6ce-a720f343d377" />         
   <img width="162" height="219" alt="image" src="https://github.com/user-attachments/assets/8065ab51-f0fa-4e48-85bc-942a059fb3a4" />
   <img width="285" height="359" alt="image" src="https://github.com/user-attachments/assets/84bef882-595b-4160-833c-8ea78473699e" />
   

    
















