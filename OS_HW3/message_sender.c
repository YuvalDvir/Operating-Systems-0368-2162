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

  // Validate that the correct number of command line arguments is passed
  if (argc < 3){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
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

  // Write the specified message to the message slot file
  ret_val = write(file_desc, argv[3], strlen(argv[3]));
  if (ERROR == ret_val){
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }

  // Close the device
  close(file_desc);

  // Exit the program with exit value 0
  return 0;
}
