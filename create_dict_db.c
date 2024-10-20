#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4 * 1024 * 1024
#define DICT_DELIMITER "!"

int main(void) {
  sqlite3 *db = NULL;
  int rc = sqlite3_open_v2("g2p.db", &db,
                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (rc) {
    fprintf(stderr, "Can't open/create db: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }
  assert(db != NULL);

  rc = sqlite3_exec(db,
                    "create table if not exists dict ( word text not null "
                    "primary key, spelling text not null );",
                    NULL, NULL, NULL);
  if (rc) {
    fprintf(stderr, "Can't open/create db: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  int fd = open("cmudict-0.7b", O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "%s\n", strerror(errno));
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }
  assert(fd > -1);

  char *file_buffer = (char *)malloc(BUFFER_SIZE);
  if (file_buffer == NULL) {
    fprintf(
        stderr,
        "Failed to allocate buffer dict file contents, Dict was too large!\n");
    sqlite3_close(db);
    exit(EXIT_FAILURE);
  }

  int bytes_read = read(fd, file_buffer, BUFFER_SIZE);
  if (bytes_read == -1) {
    fprintf(stderr, "Failed to read all bytes into buffer only read: %d\n",
            bytes_read);
    fprintf(stderr, "%s\n", strerror(errno));
    sqlite3_close(db);
    free(file_buffer);
    exit(EXIT_FAILURE);
  }

  char *dict_str = strpbrk(file_buffer, DICT_DELIMITER);
  assert(dict_str != NULL);

  // printf("word: %s\n", word);
  // printf("spelling: %s\n", spelling);
  char *word, *spelling;
  
  sqlite3_stmt *stmt;
  rc = sqlite3_prepare_v2(db, "insert into dict (word, spelling) values (?, ?);", -1, &stmt, 0);
  while (((word = strsep(&dict_str, " ")) != NULL) &&
         ((spelling = strsep(&dict_str, "\n")) != NULL)) {

    int word_rc = sqlite3_bind_text(stmt, 1, word, -1, SQLITE_STATIC);
    int spelling_rc = sqlite3_bind_text(stmt, 2, spelling, -1, SQLITE_STATIC);
    if ( word_rc != SQLITE_OK || spelling_rc != SQLITE_OK ) {
      fprintf(stderr, 
          "Failed to write values word = %s, spelling = %s\n", 
          word, spelling);
      sqlite3_close(db);
      free(file_buffer);
      exit(EXIT_FAILURE);
    }

    rc = sqlite3_step(stmt);
    if ( rc == SQLITE_ERROR ) {
      fprintf(stderr, "Failed to execute prepared statment: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      free(file_buffer);
      exit(EXIT_FAILURE);
    }

    sqlite3_reset(stmt);
    printf("wrote word = %s, spelling = %s to db", word, spelling);
  }

  rc = sqlite3_finalize(stmt);
  if ( rc  != SQLITE_OK ) {
    fprintf(stderr, "Failed to finalize statement: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    free(file_buffer);
    exit(EXIT_FAILURE);
  }

  if ( file_buffer != NULL ) free(file_buffer);
  sqlite3_close(db);
  exit(EXIT_SUCCESS);
}
