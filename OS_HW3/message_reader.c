#include "message_slot.h"

#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>

int main()
{
  int file_desc;
  int ret_val;
  char buf[BUF_SIZE];

  // Validate that the correct number of command line arguments is passed
  if (argc < 2){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1)
  }

  // Open the specified message slot device file
  file_desc = open(argv[1], O_RDWR );
  if( file_desc < 0 )
  {
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }

  // Set the channel id to the id specified on the command line
  ret_val = ioctl(file_desc, MSG_SLOT_CHANNEL, atoi(argv[2]));
  if (ERROR == ret_val){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }

  // Read a message from the message slot file to a buffer
  ret_val = read(file_desc, buf, BUF_SIZE);
  if (ERROR == ret_val){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }

  // Print the message to standard output
  ret_val = write(stdout,buf, ret_val);
  if (ERROR == ret_val){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }

  // Close the device
  close(file_desc);

  // Exit the program with exit value 0
  return 0;
}
