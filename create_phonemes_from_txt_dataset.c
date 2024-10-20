#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>


int main(void) {
  struct stat st = {0};
  char *path_to_outdir = "/home/chr1swill/Downloads/phoneme_phases";
  int outdir_fd = -1;

  if (stat(path_to_outdir, &st) == -1) {
    outdir_fd = mkdir(path_to_outdir, S_IRWXU);
    if (outdir_fd == -1) {
      fprintf(stderr, "ERROR making directory: %s\n", strerror(errno));
      return 1;
    }
  }

  outdir_fd = open(path_to_outdir, O_DIRECTORY);
  if (outdir_fd == -1 ) {
    fprintf(stderr, "ERROR opening: %s\n", strerror(errno));
    return 1;
  }

  char *path_to_textdir = "/home/chr1swill/Downloads/VCTK-Corpus/txt";
  DIR *textdir = opendir(path_to_textdir);
  if (textdir == NULL ) {
    fprintf(stderr, "ERROR opening: %s\n", strerror(errno));
    return 1;
  }

  struct dirent *dir;
  // unsigned short int d_reclen;
  // unsigned char d_type;
  // char d_name[256];		/* We must not include limits.h! */
  
  // make it work for one then parallelize it once you understand what you are doing
  dir = readdir(textdir);
  assert( dir->d_name != NULL );
  printf("filename = (%s), type = (%d), reclen = (%d)\n", dir->d_name, dir->d_type, dir->d_reclen);

  return 0;
}
