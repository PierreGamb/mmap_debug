#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#define FRAME_BUFFER "/dev/fb0"

int main(){

  int N=5;
  int fb_dev = open(FRAME_BUFFER, O_RDWR);

  int *ptr = mmap ( NULL, N*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fb_dev , 0 );
  if(ptr == MAP_FAILED){
    perror("Mapping Failed\n");
    return 1;
  }
  int i;

  for(i=0; i<N; i++)
      ptr[i] = i;

  for(i=0; i<N; i++)
      printf("[%d] ",ptr[i]);

  printf("\n");

  munmap(ptr, 10*sizeof(int));


  return 0;
}