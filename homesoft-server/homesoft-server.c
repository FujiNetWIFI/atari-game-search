#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define RESULTS_MAX 128
#define RESULTS_SIZE 128

char results[RESULTS_MAX][RESULTS_SIZE];

char argv_path[256];

unsigned int num_results=0;

char ascii_to_atari(char c)
{
    c &= 0x7F;

    if (c<32)
        c+=64;
    else if (c<96)
        c-=32;

    return c;
}

void
search_directory (const char *path, const char *target_lower)
{
  DIR *dir = opendir (path);

  if (!dir)
    {
      fprintf (stderr, "Error opening %s: %s\n", path, strerror (errno));
      return;
    }

  struct dirent *entry;
  while ((entry = readdir (dir)))
    {
      if (strcmp (entry->d_name, ".") == 0
	  || strcmp (entry->d_name, "..") == 0)
	continue;

      char fullpath[RESULTS_SIZE];
      memset(fullpath,0,sizeof(fullpath));

      int path_len =
	snprintf (fullpath, sizeof (fullpath), "%s/%s", path, entry->d_name);
      if (path_len < 0 || (size_t) path_len >= sizeof (fullpath))
	{
	  //fprintf (stderr, "Path too long: %s/%s\n", path, entry->d_name);
	  continue;
	}

      struct stat statbuf;
      if (lstat (fullpath, &statbuf) == -1)
	{
	  fprintf (stderr, "Error stating %s: %s\n", fullpath,
		   strerror (errno));
	  continue;
	}

      if (S_ISDIR (statbuf.st_mode))
	{
	  search_directory (fullpath, target_lower);
	}

      if (strcasestr (entry->d_name, target_lower))
	{
            if (num_results == RESULTS_MAX-1)
                break;

            strcpy(results[num_results],&fullpath[strlen(argv_path)+1]);

	    // Get rid of the null terminator, so we don't trip over it during screen code conversion.
	    results[num_results][strlen(results[num_results])] = 0x20;

	    for (int i=0;i<RESULTS_SIZE;i++)
		results[num_results][i] = ascii_to_atari(results[num_results][i]);

	    num_results++;
	}
    }

  closedir (dir);
}

void
handle_request (int client_socket)
{
  char buffer[4096] = { 0 };
  char header[3072];
  char query[256] = { 0 };

  // Clear the result buffer
  memset (results,0x20, RESULTS_MAX*RESULTS_SIZE);
  num_results = 0;

  // Read the request
  read (client_socket, buffer, sizeof(buffer));

  // Parse the query parameter
  char *q_param = strstr (buffer, "query=");
  if (q_param)
    {
      // Extract query value
      sscanf (q_param, "query=%255[^ &\n\r]", query);

      // URL decode the query (basic implementation)
      char *src = query;
      char *dst = query;
      while (*src)
	{
	  if (*src == '+')
	    {
	      *dst = ' ';
	    }
	  else if (*src == '%' && src[1] && src[2])
	    {
	      unsigned int value;
	      sscanf (src + 1, "%2x", &value);
	      *dst = value;
	      src += 2;
	    }
	  else
	    {
	      *dst = *src;
	    }
	  src++;
	  dst++;
	}
      *dst = '\0';
    }

  printf("query is: %s\n",query);
  search_directory(argv_path,query);

  // Prepare response
  sprintf (header,
	   "HTTP/1.1 200 OK\r\n"
	   "Content-Type: application/octet-stream\r\n"
	   "Connection: close\r\n"
	   "Content-Length: %u\r\n"
	   "\r\n",
	   num_results * RESULTS_SIZE);

  // Send response
  write (client_socket, header, strlen (header));
  write (client_socket, results, num_results * RESULTS_SIZE);
}

int
main (int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"%s <port-number> <path>\n",argv[0]);
        return(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    strncpy(argv_path,argv[2],sizeof(argv_path)-1);

  int server_fd, client_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof (address);

  // Create socket
  if ((server_fd = socket (AF_INET, SOCK_STREAM, 0)) == 0)
    {
      perror ("Socket creation failed");
      exit (EXIT_FAILURE);
    }

  // Set socket options
  if (setsockopt (server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)))
    {
      perror ("Setsockopt failed");
      exit (EXIT_FAILURE);
    }

  // Setup address structure
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons (port);

  // Bind socket
  if (bind (server_fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      perror ("Bind failed");
      exit (EXIT_FAILURE);
    }

  // Listen for connections
  if (listen (server_fd, 3) < 0)
    {
      perror ("Listen failed");
      exit (EXIT_FAILURE);
    }


  printf ("Homesoft Server listening on port %d for path %s...\n", port,argv_path);

  while (1)
    {
      // Accept connection
      if ((client_socket = accept (server_fd, (struct sockaddr *) &address,
				   (socklen_t *) & addrlen)) < 0)
	{
	  perror ("Accept failed");
	  continue;
	}

      handle_request (client_socket);
      close (client_socket);
    }

  return 0;
}
