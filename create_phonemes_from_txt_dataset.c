#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

char *join_path(char *filepath, char *filename) {
  char *path_to_file = strdup(filepath);
  if (path_to_file == NULL) {
    fprintf(stderr, "Error duplicating path: %s\n", strerror(errno));
    return NULL;
  }

  printf("path to file = (%s)\n", filepath);
  printf("filename = (%s)\n", filename);

  int filepath_buffer_size = (strlen(filepath) + 1 + strlen(filename) + 1);
  char *filepath_buffer = malloc(sizeof(*filepath_buffer) * filepath_buffer_size);
  if (filepath_buffer == NULL) {
    fprintf(stderr, "Error creating buffer for filepath: %s\n", strerror(errno));
    return NULL;
  }

  int rc = snprintf(filepath_buffer, filepath_buffer_size,
      "%s/%s", filepath, filename);
  if (rc == -1) {
    fprintf(stderr, "Error creating file path: %s\n", strerror(errno));
    free(filepath_buffer);
    return NULL;
  }

  return filepath_buffer;
};

struct list_node {
  char *string;
  struct list_node *next;
};

void str_toupper(char* string, int length) {
  assert(string[length] == '\0');
  int i = 0;
  while (i < length) {
    string[i] = toupper(string[i]);

    i++;
  }
}

char *to_phonetic_tokens(char *string) {
  int length = strlen(string);
  assert(string[length] == '\0');

  str_toupper(string, length);

  struct list_node head = {0};
  for (int i = 0; i < length; i++) {
    char letter =  string[i];
    if ((isspace(letter)) != 0) {
      printf("got space = (%c)\n", letter); 
      continue;
    } else if ((ispunct(letter)) != 0) {
      printf("got punction = (%c)\n", letter); 
      continue;
    } else if ((isdigit(letter)) != 0) {
      printf("got digit = (%c)\n", letter); 
      continue;
    } else if ((isalpha(letter)) != 0) {
      printf("got alpha = (%c)\n", letter); 
      continue;
    } else {
      printf("no handler for char = (%c)\n", letter); 
      continue;
    }
  }
  return "";
}

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
  printf("dir name = (%s)\n", dir->d_name);

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

    char *filepath = join_path(path_to_file, speaker_dir_file->d_name);
    if (filepath == NULL) {
      fprintf(stderr, "Error failed to created filepath\n");

      close(outdir_fd);
      closedir(speaker_text_dir);
      closedir(textdir);
      free(pathname);
      free(path_to_file);
      return 1;
    }

    printf("full file path = (%s)\n", filepath);

    int textfile_fd = open(filepath, O_RDONLY);
    if (textfile_fd == -1) {
      fprintf(stderr, "Error opening textfile = (%s): %s\n", filepath, strerror(errno));

      close(outdir_fd);
      closedir(speaker_text_dir);
      closedir(textdir);
      free(pathname);
      free(path_to_file);
      free(filepath);
      return 1;
    }

    struct stat st = {0};
    int result = stat(filepath, &st);
    assert(result != -1);

    char *file_content = (char *)mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, textfile_fd, 0);
    if (file_content == MAP_FAILED) {
      fprintf(stderr, "Error maping file content to memory: %s\n", strerror(errno));

      close(outdir_fd);
      closedir(speaker_text_dir);
      closedir(textdir);
      free(pathname);
      free(path_to_file);
      free(filepath);
      close(textfile_fd);
      return 1;
    }
    file_content[st.st_size-1] = '\0'; 
    // read to convert senetence to phenome senetence x

    char *phonetic_tokens = to_phonetic_tokens(file_content);
                                     
    printf("file content = (%s)\n", file_content);

    free(path_to_file);
    free(filepath);
    close(textfile_fd);
    munmap(file_content, st.st_size);
    break;
  }

  close(outdir_fd);
  closedir(speaker_text_dir);
  closedir(textdir);
  free(pathname);

  return 0;
}
