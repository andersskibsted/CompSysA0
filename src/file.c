#include <stdio.h>  // fprintf, stdout, stderr.
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS.
#include <string.h> // strerror.
#include <errno.h>  // errno.

int print_hello_world(void) {
  return fprintf(stdout, "Hello, world!\n");
}

int print_error(char *path, int errnum) {
  return fprintf(stdout,
                 "%s: cannot determine (%s)\n",
                 path,
                 strerror(errnum));
}

int is_ascii_char(unsigned char c) {
  if ((c >= 7 && c <= 13) || c == 27 || (c >= 32 && c <= 127)) {
    return 1;
  } else {
    return 0;
  }
}

int is_iso_char(unsigned char c) {
  if (is_ascii_char(c) || (c > 159 && c > 0)) {
    return 1;
  } else {
    return 0;
  }
}

int is_utf_char(unsigned char c) {
  // Missing implementation
  if (c >= 128 && c <= 191) {
    return 1;
  } else if (c <= 247 && c > 0) {
    return 1;
  } else {
    return 0;
  }
}

int is_data_char(char c) {
  // Missing implementation
  if (c) {
    return 0;
  }
  return 0;
}

int is_empty_char(char c) {
  // Missing implementation
  if (c) {
    return 0;
  }
  return 0;
}

enum file_type {
  DATA,
  EMPTY,
  ASCII,
  ISO,
  UTF
};

const char * const FILE_TYPE_STRINGS[] = {
  "data",
  "empty",
  "ASCII text",
  "ISO-8859 text",
  "Unicode text, UTF-8 text, with no line terminators"
};

int main(int argc, char* argv[]) {
  // Sanitizing input
  if (argc == 2) {

    FILE *f = fopen(argv[1], "r");
    if (errno) {
      print_error(argv[1], errno);
      return EXIT_SUCCESS;
    }

    unsigned char c = '\0';
    int is_ascii = 1;
    int is_iso = 1;
    int is_utf = 1;
    int n = fread(&c, sizeof(char), 1, f);

    if (n == 0) {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[EMPTY]);
      return EXIT_SUCCESS;
    }

    while (n == 1) {
      is_ascii = is_ascii && is_ascii_char(c);
      is_iso = is_iso && is_iso_char(c);
      is_utf = is_utf && is_utf_char(c);
      n = fread(&c, sizeof(char), 1, f);
    }


    fclose(f);
    if (is_ascii) {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[ASCII]);
    } else if (is_utf) {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[UTF]);
    } else if (is_iso) {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[ISO]);
    } else {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[DATA]);
    }

  } else {
    fprintf(stderr, "Usage: file path\n");
    return EXIT_FAILURE;
  }


  //if (argc == 2) {
    //fprintf(stdout, "Succes %s\n", argv[1]);
   // retval = EXIT_SUCCESS;
  //}
  //if (print_hello_world() <= 0) {
  //  retval = EXIT_FAILURE;
  //}

}
