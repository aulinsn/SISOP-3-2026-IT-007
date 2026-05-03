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

   











