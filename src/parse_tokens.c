#include <stdio.h>
#include <string.h>

#define MAX_TOKENS (128)
#define MAX_TOKEN_LEN (1024)

#define SINGLE_QUOTE '\''
#define DOUBLE_QUOTE '\"'
#define SPACE ' '
#define BACKSLASH '\\'

enum parse_state
{
  LOOKING_FOR_SPACE,
  FOUND_DOUBLE_QUOTE,
  FOUND_DOUBLE_QUOTE_ESCAPE,
  FOUND_SINGLE_QUOTE,
  FOUND_SLASH,
};

static char tokens[MAX_TOKENS][MAX_TOKEN_LEN];

static bool parse_input(char* input, int input_len, int* num_tokens)
{
  char* cur_token;
  int tokens_idx = 0;
  int token_idx = 0;
  bool res = true;
  enum parse_state state = LOOKING_FOR_SPACE;

  memset(tokens, 0, sizeof(tokens));
  cur_token = tokens[tokens_idx++];

  for (int i = 0; i < input_len; i++)
  {
    char cur_char = input[i];
    bool copy_char = false;

    if (tokens_idx >= MAX_TOKENS)
    {
      res = false;
      break;
    }

    if (token_idx >= MAX_TOKEN_LEN)
    {
      res = false;
      break;
    }

    switch (state)
    {
      case LOOKING_FOR_SPACE:
        // Found a space
        if (cur_char == SPACE)
        {
          // Current character is a space and previous isn't a space start a new token
          if (input[i - 1] != SPACE)
          {
            token_idx = 0;
            cur_token = tokens[tokens_idx++];
          }
          break;
        }

        // Found a backslash escape sequence
        if (cur_char == '\\')
        {
          state = FOUND_SLASH;
          break;
        }

        // Start a quoted sequence
        if (cur_char == SINGLE_QUOTE || cur_char == DOUBLE_QUOTE)
        {
          state = (cur_char == SINGLE_QUOTE) ? FOUND_SINGLE_QUOTE : FOUND_DOUBLE_QUOTE;
          break;
        }

        // Not a special character, copy character to token buffer
        copy_char = true;
        break;

      case FOUND_SINGLE_QUOTE:
        // Found end single quote, go back to looking for space state
        if (cur_char == SINGLE_QUOTE)
        {
          state = LOOKING_FOR_SPACE;
          break;
        }

        // Still inside quoted test, copy character to token buffer
        copy_char = true;
        break;

      case FOUND_DOUBLE_QUOTE:
        if (cur_char == BACKSLASH)
        {
          char escaped = input[i + 1];
          if (escaped == BACKSLASH ||
              escaped == DOUBLE_QUOTE)
          {
            state = FOUND_DOUBLE_QUOTE_ESCAPE;
            break;
          }
        }

        // Found end double quote, go back to looking for space state
        if (cur_char == DOUBLE_QUOTE)
        {
          state = LOOKING_FOR_SPACE;
          break;
        }

        // Still inside quoted test, copy character to token buffer
        copy_char = true;
        break;

      case FOUND_DOUBLE_QUOTE_ESCAPE:
        // Copy character after slash, go back to looking for space state
        copy_char = true;
        state = FOUND_DOUBLE_QUOTE;
        break;

      case FOUND_SLASH:
        // Copy character after slash, go back to looking for space state
        copy_char = true;
        state = LOOKING_FOR_SPACE;
        break;
    }

    if (copy_char == true)
    {
      cur_token[token_idx++] = cur_char;
    }
  }

  *num_tokens = tokens_idx;

  return res;
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

  if (out_tokens == NULL || tokens_len <= 0 || num_tokens == NULL)
  {
    printf("Incorrect params: tokens 0x%p, len %d, num_tokens 0x%p\n",
           out_tokens,
           tokens_len,
           num_tokens);
    return false;
  }

  parse_input(input, input_len, num_tokens);

  for (int idx = 0; idx < *num_tokens && idx < tokens_len && idx < MAX_TOKEN_LEN; idx++)
  {
    out_tokens[token_idx++] = tokens[idx];
  }

  out_tokens[token_idx] = NULL;

  return true;
}
