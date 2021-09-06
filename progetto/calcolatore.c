#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define NUM_THREAD 3

//------------------------------------------------------------------DICHIARAZIONI

int *arrNum; //un array che contiene tutti numeri
int max; //contenga il valore passato da linea di comando
int count; //conta il # dei numeri primi
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *isPrime();

//-----------------------------------------------------------------DEFINIZIONI FUNZIONI

//funzione che verifica se un numero è un numero primo o meno, poi li "memorizza" in un array
void *isPrime(void *n){

  int flag; //segnala se un numero è primo o no
  long corrente = (long)n;

  printf("Entering thread[%ld]\n", pthread_self());
  pthread_mutex_lock(&mutex);
  while(corrente <= max){
    printf("Thread[%ld] checking %ld\n", pthread_self(), corrente);
    flag = 0;
    for(int i = 2; i <= sqrt(corrente); i++){
      //se il numero corrente è divisibile per un qualsiasi numero , allora segnalarlo come non primo e esce dal ciclo
      if(corrente % i == 0){
        flag = 1;
        break;
      }
    }//end for

    //se alla fine del ciclo, il numero corrente ha flag = 0(non segnalato) e che sia maggiore di 1, allora $corrente è un numero primo
    if(flag == 0 && corrente > 1){
      count++;
      arrNum[corrente] = 1;
    }
    //ogni thread verifica un numero che sia il moltiplo di 3 + 0/1/2
    corrente = corrente + NUM_THREAD;
  }//end while

  pthread_mutex_unlock(&mutex);
  printf("Exiting thread[%ld]\n", pthread_self());
  return 0; //termina il thread corrente

}

//--------------------------------------------------------------------MAIN
int main(int argc, char *argv[]){

  //richiede che utente passa soltanto 2 argomento tramite linea di comendo
  if(argc != 2){
    fprintf(stderr, "Usage: %s <integer> <file_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  max = atoi(argv[1]);
  pthread_t thread[NUM_THREAD];
  arrNum = malloc(max * sizeof(int*));
  long corrente; //la variabile indica il numero corrente quale thread stia verificando
  count = 0; //conta il numero dei numeri primi
  int dim; //una copia di count
  int *numPrimi; //array che contiene soltanto i numeri primi
  int temp = 0; //è un indice che serve per ripartire array da dove era fermato
  int fd; //file descriptor del FIFO myfifo
  char *myfifo = "/tmp/myfifo"; //path di myfifo

  if(max <= 1){
    //richiede che utente inserisca un intero positivo maggiore di 1
    printf("Warning: expect an integer greater than 1\n");
  }else{
    //inizializzazione dell'array che contenga $num numeri
    for(int i = 0; i <= max; i++){
      arrNum[i] = 0;
    }

    printf("Creating threads\n");
    for(corrente = 0; corrente < NUM_THREAD; corrente++){
      pthread_create(&thread[corrente], NULL, isPrime, (void*)corrente);
      printf("thread n.%ld - [%ld]\n", corrente, thread[corrente]);
    }
    puts("");

    //Joining threads
    for(int i = 0; i < NUM_THREAD; i++){
      pthread_join(thread[i], NULL);
    }

    dim = count; //dim prende il valore di count
    //dinamicamente assegnare numPrimi e inizializzarlo
    numPrimi = malloc(dim * sizeof(int*));
    for(int i = 0; i < dim; i++){
      numPrimi[i] = 0;
    }

    //definire numPrimi che contiene solo i numeri primi
    for(int i = 0; i < count; i++){
      for(int j = temp; j <= max; j++){
        temp++;
        //se numPrimi[j] == 1 significa che j è un numero primo
        //allora va memorizzato in un altro array che contiene solo numeri primi
        if(arrNum[j] == 1){
          numPrimi[i] = j;
          break; //serve per aumentare indice i, grazie al temp, indice j riparte da dove era fermato
        }
      }
    }

    //creazione FIFO myfifo
    printf("\nCreating a FIFO\n");
    if((mkfifo(myfifo, 0666)) == -1){
      perror("Create myfifo");
      exit(EXIT_FAILURE);
    }else{
      //se successo, apertura myfifo
      fd = open(myfifo, O_WRONLY);
      if(fd == -1){
        perror("Open myfifo");
        exit(EXIT_FAILURE);
      }else{
        //se successo, scrittura in myfiflo
        write(fd, &dim, sizeof(int)); //invia la dimensione dell'array che contenga solo i numeri primi
        printf("Sent dim = %d\n", dim);
        for(int i = 0; i < dim; i++){ //invia il contenuto di numPrimi una posizione alla volta
          write(fd, &numPrimi[i], sizeof(int));
        }
        //close(fd);
      }
    }
    close(fd);

    printf("Sent array of prime numbers: ");
    for(int i = 0; i < dim; i++){
      printf("%d ", numPrimi[i]);
    }
    puts("");

    printf("Closing FIFO and freeing dynamically allocated arrays\n");
    //elimina FIFO myfifo una volta iniviati e letti i dati
    unlink(myfifo);
    //libera i array dinamicamente allocati
    free(arrNum);
    free(numPrimi);
    arrNum = NULL;
    numPrimi = NULL;
  }//end else

  return 0;

}//end main
