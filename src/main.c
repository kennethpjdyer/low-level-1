#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<getopt.h>

#include"parse.h"
#include"file.h"
#include"common.h"

void print_usage(char* argv[]){
   printf(
      "Usage: %s -n -f <database file>\n"
      "\t -f - (required) path to databas file\n"
      "\t -a - adds a new employee to the database\n"
      "\t -n - create new database file\n"
      , argv[0]);

   return;
}

int main(int argc, char** argv){

   char* addString = NULL;
   char* filepath = NULL;
   bool newfile = false;
   int dbfd = -1;
   struct dbheader_t* dbhdr = NULL;
   struct employee_t* employees = NULL;

   int c;
   while ((c = getopt(argc, argv, "nf:a:")) != -1) {
      switch (c) {
         case 'n':
         newfile = true;
         break;

         case 'f':
         filepath = optarg;
         break;

         case 'a':
         addString = optarg;
         break;

         case '?':
         printf("Unknown option -%c\n", c);
         break;

         default:
         return -1;
      }
   }

   // Required Options
   if (filepath == NULL){
      printf("Filepath is a required argument\n");
      print_usage(argv);

      return 0;
   }

   if (newfile){
      dbfd = create_db_file(filepath);
      if (dbfd == STATUS_ERROR){
         printf("Unable to create database file\n");
         return -1;
      }
      if (create_db_header(&dbhdr) == STATUS_ERROR){
         printf("Unable to create database header\n");
         return -1;
      }
   } else {
      dbfd = open_db_file(filepath);
      if (dbfd == STATUS_ERROR){ 
         printf("Unable to open database file\n");
         return -1;
      }
      if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR){
         printf("Failed to validate database header\n");
         return -1;
      }
   }

   if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
      printf("Failed to read employees");
      return 0;
   }

   // Add Employee
   if (addString){
      add_employee(dbhdr, &employees, addString);
   }

   // Write Database to Disk
   output_file(dbfd, dbhdr, employees);

   return 0;
}
