#include <stdio.h>

#include "main.h"

__attribute__((noreturn)) void _exit(int status) {
  if (status) {
    Error_Handler();
  } else {
    while (1) {
    }
  }
}
