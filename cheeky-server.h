#ifndef H_CHEEKY_SERVER
#define H_CHEEKY_SERVER

#define PORT      3490
#define BACKLOG   4
#define SHM_KEY   "cheekyled_key"
#define SHM_SIZE  1024
#define BUFF_SIZE 500

void set_message(char *msg);

#endif