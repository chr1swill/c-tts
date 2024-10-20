#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


typedef struct node {
  char *token;
  struct node *next;
} list_node;

list_node *create_node(char *token) {
  list_node *new_node = (list_node *)malloc(sizeof(list_node));
  if (new_node == NULL) {
    return NULL;
  }

  new_node->token = strdup(token);
  if (new_node->token == NULL) {
    free(new_node);
    return NULL;
  }

  new_node->next = NULL;
  return new_node;
}

// with link list you need to always maintain the head or else you have no idea where the list starts
list_node *str_to_token_list(char *str) {
  char *token = NULL;

  list_node *head = NULL; 
  list_node *curr = NULL;
  
  /*
   * TODO: need to encode as string token
   *
   * : <char_colon>
   * ; <char_semi_colon>
   * , <char_comma>
   * . <char_period>
   * ! <char_emphasis>
   * ? <char_question>
   * / <char_forward_slash>
   * " <char_double_quote>
   * ' <char_single_quote>
   * % <char_percent>
   * + <char_plus>
   * = <char_equals>
   * ( <char_open_bracket>
   * ) <char_closed_bracket>
   */

  // chars that will match the all isspace() + some punctions that are not wanted  
  const char *delimiter = " \t\n\v\f\r[]{}\\'\"-|<>*~`"; /* you may need to expand '=', '+',  '%', along side numbers  */

  for( token=strtok(str, delimiter); token; token=strtok(NULL, delimiter) ) {
    list_node *new_node = create_node(token);
    if ( head == NULL ) {
      head = new_node;
      curr = new_node;
    } else {
      assert(token != NULL);
      curr->next = new_node;
      curr = curr->next;
    }
  }

  return head; 
}

void print_list(list_node *head) {
  assert( head != NULL );

  list_node *tmp = head;

  for (; tmp; tmp = tmp->next) {
    assert( tmp->token != NULL );
    printf("tok: %s\n", tmp->token);
  }
} 

#define ITTER_LIST_CB(lh, cb) \
  for (; (lh) != NULL; (lh) = (lh)->next) { \
    assert( (lh)->token != NULL ); \
    cb( (lh)->token, strlen( (lh)->token ) );\
  } \

#define FREE_LIST(lh)\
  while (lh != NULL) {\
    list_node *tmp = (lh);\
    (lh) = (lh)->next;\
    assert(tmp->token != NULL);\
    free(tmp->token);\
    free(tmp);\
  }\


/**
 * @brief find first digit char, uses reverse indexing 
 * @return index of digit charater or -1 if none was found
 */
int token_has_digit(char *token, int token_length) {
  while (token_length--) {
    if ( isdigit(token[token_length]) ) return token_length;
  }
  return -1;
}

char *map_digit_to_str(char digit) {
    switch ( digit ){
      case '1':
        return "ONE";
      case '2':
        return "TWO";
      case '3':
        return "THREE";
      case '4':
        return "FOUR";
      case '5':
        return "FIVE";
      case '6':
        return "SIX";
      case '7':
        return "SEVEN";
      case '8':
        return "EIGHT";
      case '9':
        return "NINE";
      case '0':
        return "ZERO";
      default:
        return NULL;
    }
}

void expand_number_token(char *token, int token_length) {
  int digit_array_length = 0;
  int digit_array[token_length];
  memset(digit_array, 0, token_length);

  int token_len = token_length;
  while ( token_len-- ) {
    if ( isdigit(token[token_len]) ) {
      assert( digit_array_length <= token_len );
      digit_array[digit_array_length] = token_len;
      digit_array_length++;
    }
  }

  // TODO: to something that will handle all type of numbers and will return them
  // correted for there context for example:
  // 11 -> "ELEVEN", instead of "ONEONE" or "ONE ONE" with you current algo
  while ( digit_array_length-- ) {
    char *str = map_digit_to_str( token[digit_array[digit_array_length]] );
    assert( str != NULL );
    token = (char *)realloc( token, token_length + strlen(str) );
    if ( token != NULL ) {
      return;
    }
  }
}

void capitalize_token(char *token, int token_length) {
  int token_len = token_length;
  if ( token_has_digit(token, token_len) != -1 ) {

  }

  while (token_length--) {
    token[token_length] = toupper(token[token_length]);
  }
}

void print_token(char *token, int token_length) {
  printf("token = (%s), length = (%d)\n", token, token_length);
}

#define MAX_SIZE 512000

int main(void) {
 char *buffer = malloc( sizeof(*buffer) *  MAX_SIZE );
  if ( buffer == NULL ) {
    return 1;
  }
  memset(buffer, 0, MAX_SIZE);
  assert( buffer[0] == 0 && buffer[MAX_SIZE-1] == 0);

  size_t bytes_read = fread(buffer, sizeof(*buffer), MAX_SIZE, stdin);
  assert( bytes_read < MAX_SIZE && bytes_read > 0 );

  puts("Got input:");
  list_node *token_list = str_to_token_list(buffer);
  
  list_node *head_n = token_list;
  //list_node *head_p = token_list;
  list_node *free_head = token_list;

  ITTER_LIST_CB(head_n, capitalize_token);
  //ITTER_LIST_CB(head_p, print_token);

  FREE_LIST(free_head);
  free(buffer);
  return 0;
}
