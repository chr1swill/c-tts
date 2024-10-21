#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

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

    close(outdir_fd);
    return 1;
  }

  struct dirent *dir;
  // unsigned short int d_reclen;
  // unsigned char d_type;
  // char d_name[256];		/* We must not include limits.h! */
  
  // make it work for one then parallelize it once you understand what you are doing
  dir = readdir(textdir);
  assert( dir->d_name != NULL );
  printf("%s\n", dir->d_name);

  char *pathname = malloc(sizeof(char) * ( strlen(path_to_textdir) + 1 + strlen(dir->d_name) + 1 ));
  if (pathname == NULL) {
    fprintf(stderr, "ERROR: failed to allocated memory for pathname\n");

    close(outdir_fd);
    closedir(textdir);
    return 1;
  }

  snprintf(pathname, (strlen(path_to_textdir) + 1 + strlen(dir->d_name) + 1),
      "%s/%s", path_to_textdir, dir->d_name);

  DIR *speaker_text_dir = opendir(pathname);
  if (speaker_text_dir == NULL ) {
    fprintf(stderr, "ERROR opening: %s\n", strerror(errno));

    close(outdir_fd);
    closedir(textdir);
    free(pathname);
    return 1;
  }

  // loop over text files in speaker directory
  struct dirent *speaker_dir_file;
  while (( speaker_dir_file = readdir(speaker_text_dir)) != NULL ) {
    assert(speaker_dir_file->d_name != NULL);

    if (speaker_dir_file->d_type != DT_REG ||
        (strcmp(speaker_dir_file->d_name, ".")) == 0 || 
        (strcmp(speaker_dir_file->d_name, "..")) == 0) {
      // skip the '.' and '..' dirs
      continue;
    };

    char *path_to_file = strdup(pathname);
    if (path_to_file == NULL) {
      fprintf(stderr, "Error duplicating path: %s\n", strerror(errno));

      close(outdir_fd);
      closedir(speaker_text_dir);
      closedir(textdir);
      free(pathname);
      return 1;
    }
    printf("path to file = (%s)\n", path_to_file);
    printf("filename = (%s)\n", speaker_dir_file->d_name);

    int filepath_buffer_size = (strlen(path_to_file) + strlen(speaker_dir_file->d_name) + 1);
    char *filepath_buffer = malloc(sizeof(*filepath_buffer) * filepath_buffer_size);

    int rc = snprintf(filepath_buffer, filepath_buffer_size,
        "%s/%s", path_to_file, speaker_dir_file->d_name);
    if (rc == -1) {
      fprintf(stderr, "Error creating file path: %s\n", strerror(errno));

      close(outdir_fd);
      closedir(speaker_text_dir);
      closedir(textdir);
      free(pathname);
      free(path_to_file);
      free(filepath_buffer);
      return 1;
    }

    printf("full file path = (%s)\n", filepath_buffer);
    free(path_to_file);
    free(filepath_buffer);
    break;
  }

  close(outdir_fd);
  closedir(speaker_text_dir);
  closedir(textdir);
  free(pathname);

  return 0;
}
