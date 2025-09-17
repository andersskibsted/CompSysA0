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

//int combine_bytes_to_int()

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

int is_second_utf_byte(unsigned char c) {
  return (c >= 128 && c <= 191);
}

int is_in_range_inclusive(unsigned char c, unsigned int lower, unsigned int upper) {
  return ((lower <= c) && (c >= upper));
}

int is_utf_char2(unsigned char* c) {
  /* for (int i = 0; i < 4; i++) { */
  /*   if (c[i] == 0) return 0; */
  /* } */

  if (c[0] <= 127) {

    return (!is_second_utf_byte(c[1]) && c[1] > 0);

  } else if ((192 <= c[0]) && (c[0] <= 223)) {
    // second byte check
    return is_second_utf_byte(c[1]);

  } else if ((223 < c[0]) && (c[0] <= 239)) {

    return (is_second_utf_byte(c[1]) && is_second_utf_byte(c[2]));

  } // Ditto
  else if ((239 < c[0]) && (c[0] <= 247)) {

    return (is_second_utf_byte(c[1]) && is_second_utf_byte(c[2]) && is_second_utf_byte(c[3]));

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

    //unsigned char c = '\0';
    unsigned char c = 0;
    //unsigned char* cp = (unsigned char*) &c;
    unsigned char byteArray[4] = { 0, 0, 0, 0 };
    int number_of_reads = 0;
    int is_ascii = 1;
    int is_iso = 1;
    int is_utf = 1;
    //int n = fread(&c, sizeof(int), 1, f);
    int n = fread(&c, sizeof(char), 1, f);


    if (n == 0) {
      printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[EMPTY]);
      return EXIT_SUCCESS;
    }

    while (n == 1) {
      number_of_reads++;

      // Move all elements in byte array
      for (int i = 1; i < 4; i++) {
        byteArray[i - 1] = byteArray[i];
      }
      // Put newest byte in last place in array;
      byteArray[3] = c;
      // Check for ASCII and ISO bytes
      is_ascii = is_ascii && is_ascii_char(c);
      is_iso = is_iso && is_iso_char(c);

      if ((number_of_reads % 4) == 0) is_utf = is_utf && is_utf_char2(byteArray);

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
