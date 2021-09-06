#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SIZE 1024

int main(int argc, char **argv){


  if(argc != 2){
    fprintf(stderr, "Usage: %s <file_name.txt>\n", argv[0]);
    exit(EXIT_FAILURE);
  }


  int dim, fd;
  int buf[SIZE]; //un buffer che memorizza i datti letti tramite FIFO
  char *myfifo = "/tmp/myfifo"; //nome di FIFO
  FILE *file; //un file
  char file_name[] = "output.txt";

  printf("Connecting to FIFO\n");
  fd = open(myfifo, O_RDONLY); //connessione al myfifo
  if(fd == -1){
    perror("Open myfifo");
    exit(EXIT_FAILURE);
  }else{ //se successo, lettura dal myfifo
    read(fd, &dim, sizeof(int));
    printf("Recieved size of array [dim] = %d\n", dim);
    read(fd, buf, dim * sizeof(int));
    close(fd);
  }

  printf("Array of prime numbers received\n");

  /*
  for(int i = 0; i < dim; i++){
    printf("%d ", buf[i]);
  }
  puts("");
  */

  //apertura e scritura in file
  file = fopen(argv[1], "a"); //apertura di del file
  printf("Writing into file [%s]: ", argv[1]);
  for(int i = 0; i < dim; i++){
    printf("%d ", buf[i]); //visualizzare il contenuto nel buffer
    fprintf(file, "%d\n", buf[i]); //scrittura del contenuto di buffer in file
  }
  fclose(file);
  printf("\nClosing file\n");

}//end main
