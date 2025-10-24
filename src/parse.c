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
   struct dbheader_t** headerOut
){

   struct dbheader_t* header = calloc(1, sizeof(struct dbheader_t));
   if (header == NULL){
      printf("malloc failed to create header\n");
      return STATUS_ERROR;
   }

   unsigned short version = 0x1;
   header->version = version;

   unsigned short count = 0;
   header->count = count;

   unsigned int m = HEADER_MAGIC;
   header->magic = m;

   unsigned int s = sizeof(struct dbheader_t);
   header->filesize = s;

   *headerOut = header;

   return STATUS_SUCCESS;
}


int output_file(
   int fd,
   struct dbheader_t* dbhdr,
   struct employee_t* employees){
   if (fd < 0){
      printf("Bad FD from user\n");
      return STATUS_ERROR;
   }
   // Fix Endianness
   int realcount = dbhdr->count;
   
   dbhdr->version = htons(dbhdr->version);
   dbhdr->magic = htonl(dbhdr->magic);
   dbhdr->count = htons(dbhdr->count);
   dbhdr->filesize = htonl(
      sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));


   lseek(fd, 0, SEEK_SET);
   write(fd, dbhdr, sizeof(struct dbheader_t));

   int i = 0;
   for (; i < realcount; i++){
      employees[i].hours = htonl(employees[i].hours);
      write(fd, &employees[i], sizeof(struct employee_t));
   }

   return STATUS_SUCCESS;

}


int 
read_employees
(
   int fd, 
   struct dbheader_t* dbhdr, 
   struct employee_t** employeesOut
){
   if (fd < 0){
      printf("Bad FD from user\n");
      return STATUS_ERROR;
   }
   int count = dbhdr->count;
   struct employee_t* employees = calloc(count, sizeof(struct employee_t));
   if (employees == NULL){
      printf("Malloc failed in read_employees\n");
      return STATUS_ERROR;
   }
   read(fd, employees, count * sizeof(struct employee_t));

   int i = 0;
   for (; i < count; i++){
      employees[i].hours = ntohl(employees[i].hours);
   }
   *employeesOut = employees;

   return STATUS_SUCCESS;
}

int
add_employee
(
   struct dbheader_t* dbhdr,
   struct employee_t** employees,
   char* addString
){

   if (dbhdr == NULL) return STATUS_ERROR;
   if (employees == NULL) return STATUS_ERROR;
   if (*employees == NULL) return STATUS_ERROR;

   if (addString == NULL) return STATUS_ERROR;
   
     

   char* name = strtok(addString, ",");
   if (name == NULL) return STATUS_ERROR;
   char* addr = strtok(NULL, ",");
   if (addr == NULL) return STATUS_ERROR;
  
   char* hrs = strtok(NULL, ",");
   if (hrs == NULL) return STATUS_ERROR;
   

   struct employee_t* e = *employees;
   e = realloc(e, sizeof(struct employee_t) * dbhdr->count+1);
   if (e == NULL){
      printf("unable to realloc employees\n");
      return STATUS_ERROR;
   }
   dbhdr->count++;

   int pos = dbhdr->count - 1;
   strncpy(e[pos].name, name, sizeof(e[pos].name));
   strncpy(e[pos].address, addr, sizeof(e[pos].address));
   e[pos].hours = atoi(hrs);

   *employees = e;

   return STATUS_SUCCESS;
}



