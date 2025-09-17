#include <errno.h>  // errno.
#include <stdio.h>  // fprintf, stdout, stderr.
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS.
#include <string.h> // strerror.

int print_error(char *path, int errnum) {
  return fprintf(stdout, "%s: cannot determine (%s)\n", path, strerror(errnum));
}

int file_len(FILE *file) {
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);
  return size;
}

int is_ascii_char(unsigned char c) {
  return ((c >= 7 && c <= 13) || c == 27 || (c >= 32 && c <= 126));
}

int is_iso_char(unsigned char c) {
  return (is_ascii_char(c) || (c >= 160 && c <= 255));
}

int is_utf_one_byte(unsigned char c) { return (c <= 127 && is_ascii_char(c)); }

int is_utf_two_byte(unsigned char c) {
  return (c >= 193 && c <= 223); // Excludes 192 as it carries no information
                                 // and should be a one byte
}

int is_utf_three_byte(unsigned char c) {
  return (c >= 225 && c <= 239); // Excludes 224; carries no information and
                                 // should be a two/one byte
}

int is_utf_four_byte(unsigned char c) {
  return (c >= 241 && c <= 255); // Excludes 240; carries no information and
                                 // should be a three/two/one byte
}

int is_utf_continuation(unsigned char c) { return (c >= 128 && c <= 191); }

enum file_type { DATA, EMPTY, ASCII, ISO, UTF };

const char *const FILE_TYPE_STRINGS[] = {
    "data", "empty", "ASCII text", "ISO-8859 text",
    "Unicode text, UTF-8 text, with no line terminators"};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: file path\n");
    return EXIT_FAILURE;
  }

  char *path = argv[1];
  FILE *file = fopen(path, "rb");

  if (file == NULL) {
    print_error(path, errno);
    return EXIT_SUCCESS;
  }

  int size = file_len(file);

  if (size == 0) {
    fprintf(stdout, "%s: empty\n", path);
    return EXIT_SUCCESS;
  }

  unsigned char buffer[size];
  int is_ascii = 1;
  int is_iso = 1;
  int is_utf = 1;

  fread(buffer, 1, size, file);
  fclose(file);
  for (int i = 0; i < size; i++) {
    is_ascii = is_ascii && is_ascii_char(buffer[i]);
    is_iso = is_iso && is_iso_char(buffer[i]);
    if (!is_ascii && !is_iso) {
      break;
    }
  }

  if (is_ascii) {
    fprintf(stdout, "%s: %s\n", path, FILE_TYPE_STRINGS[ASCII]);
    return EXIT_SUCCESS;
  }

  if (is_iso) {
    fprintf(stdout, "%s: %s\n", path, FILE_TYPE_STRINGS[ISO]);
    return EXIT_SUCCESS;
  }

  int i = 0;
  while (i < size) {
    if (is_utf_one_byte(buffer[i])) {
      i++;
      continue;
    }

    if ((i + 1 < size) && is_utf_two_byte(buffer[i]) &&
        is_utf_continuation(buffer[i + 1])) {
      i += 2;
      continue;
    }

    if ((i + 2 < size) && is_utf_three_byte(buffer[i]) &&
        is_utf_continuation(buffer[i + 1]) &&
        is_utf_continuation(buffer[i + 2])) {
      i += 3;
      continue;
    }

    if ((i + 3 < size) && is_utf_four_byte(buffer[i]) &&
        is_utf_continuation(buffer[i + 1]) &&
        is_utf_continuation(buffer[i + 2]) &&
        is_utf_continuation(buffer[i + 3])) {
      i += 4;
      continue;
    }
    is_utf = 0;
    break;
  }

  if (is_utf) {
    fprintf(stdout, "%s: %s\n", path, FILE_TYPE_STRINGS[UTF]);
    return EXIT_SUCCESS;
  }

  fprintf(stdout, "%s: %s\n", path, FILE_TYPE_STRINGS[DATA]);
  return EXIT_SUCCESS;
}
