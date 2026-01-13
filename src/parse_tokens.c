#include <stdio.h>
#include <string.h>

#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define SPACE ' '

enum parse_state
{
  LOOKING_FOR_SPACE,
  FOUND_DOUBLE_QUOTE,
  FOUND_SINGLE_QUOTE,
  FOUND_SLASH,
};

static char tokens[128][1024];

static bool parse_input(char* input, int input_len, int* num_tokens)
{
  char* cur_token;
  int tokens_idx = 0;
  int token_idx = 0;
  enum parse_state state = LOOKING_FOR_SPACE;

  memset(tokens, 0, sizeof(tokens));
  cur_token = tokens[tokens_idx++];

  for (int i = 0; i < input_len; i++)
  {
    char cur_char = input[i];
    switch (state)
    {
      case LOOKING_FOR_SPACE:
        if (cur_char == SPACE)
        {
          if (input[i - 1] != SPACE)
          {
            token_idx = 0;
            cur_token = tokens[tokens_idx++];
          }
          break;
        }

        if (cur_char == '\\')
        {
          state = FOUND_SLASH;
          break;
        }

        if (cur_char == SINGLE_QUOTE || cur_char == DOUBLE_QUOTE)
        {
          state = (cur_char == SINGLE_QUOTE) ? FOUND_SINGLE_QUOTE : FOUND_DOUBLE_QUOTE;
          break;
        }

        cur_token[token_idx++] = cur_char;
        break;

      case FOUND_SINGLE_QUOTE:
        if (cur_char == SINGLE_QUOTE)
        {
          state = LOOKING_FOR_SPACE;
          break;
        }

        cur_token[token_idx++] = cur_char;
        break;

      case FOUND_DOUBLE_QUOTE:
        if (cur_char == DOUBLE_QUOTE)
        {
          state = LOOKING_FOR_SPACE;
          break;
        }

        cur_token[token_idx++] = cur_char;
        break;

      case FOUND_SLASH:
        cur_token[token_idx++] = cur_char;
        state = LOOKING_FOR_SPACE;
        break;
    }
  }

  *num_tokens = tokens_idx;

  return true;
}

bool parse_tokens(char* input, int input_len, char** out_tokens, int tokens_len, int* num_tokens)
{
  int token_idx = 0;
  char* cur_token;

  if (input == NULL || input_len <= 0)
  {
    printf("Invalid input 0x%p, len %d", input, input_len);
    return false;
  }

  if (out_tokens == NULL || num_tokens == NULL || tokens_len <= 0)
  {
    printf("Incorrect params: tokens 0x%p, len %d, num_tokens 0x%p\n",
           out_tokens,
           tokens_len,
           num_tokens);
    return false;
  }

  parse_input(input, input_len, num_tokens);

  for (int idx = 0; idx < *num_tokens; idx++)
  {
    out_tokens[token_idx++] = tokens[idx];
  }

  out_tokens[token_idx] = NULL;

  return true;
}
