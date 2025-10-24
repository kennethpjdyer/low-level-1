#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

#include"common.h"
#include"parse.h"


int 
validate_db_header(
   int fd, 
   struct dbheader_t** headerOut
){
   if (fd < 0){
      printf("Bad FD from user\n");
      return STATUS_ERROR;
   }
   struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
   if (header == NULL){
      printf("Malloc failed to allocate database header\n");
      return STATUS_ERROR;
   }
   if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)){
      perror("read");
      free(header);
      return STATUS_ERROR;
   }
   header->version  = ntohs(header->version);
   header->count    = ntohs(header->count);
   header->magic    = ntohl(header->magic);
   header->filesize = ntohl(header->filesize);

   if (header->version != 1){
      printf("Improper header version: %d\n", header->version);
      free(header);
      return STATUS_ERROR;
   }

   if (header->magic != HEADER_MAGIC){
      printf("Improper header magic\n");
      free(header);
      return STATUS_ERROR;
   }
   struct stat dbstat = {0};
   fstat(fd, &dbstat);
   if (header->filesize != dbstat.st_size){
      printf("Corrupted Database\n");
      return STATUS_ERROR;
   }
   *headerOut = header;


   return STATUS_SUCCESS;
}


int 
create_db_header
(
   int fd, 
   struct dbheader_t** headerOut
){

   struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
   if (header == NULL){
      printf("malloc failed to create header\n");
      return STATUS_ERROR;
   }

   unsigned short version = 0x1;
   header->version = ntohs(version);

   unsigned short count = 0;
   header->count = ntohs(count);

   unsigned int m = HEADER_MAGIC;
   header->magic = ntohl(m);

   header->filesize = ntohl(sizeof(struct dbheader_t));

   *headerOut = header;
   output_file(fd, header);

   return STATUS_SUCCESS;
}


void output_file(
   int fd,
   struct dbheader_t* dbhdr){
   if (fd < 0){
      printf("Bad FD from user\n");
      return;
   }

   // Fix Endianness
   dbhdr->version = htons(dbhdr->version);
   dbhdr->magic = htonl(dbhdr->magic);
   dbhdr->count = htons(dbhdr->count);
   dbhdr->filesize = htonl(dbhdr->filesize);

   lseek(fd, 0, SEEK_SET);
   write(fd, dbhdr, sizeof(struct dbheader_t));
   return;

}
