#include <stdint.h>
#include <stdio.h>



int array_contains(const uint8_t *array, size_t size, int target) {
  for (size_t i = 0; i < size; i++) {
    if (array[i] == target) {
      return 1;
    }
  }
  return 0;
}

uint8_t randomByte(uint8_t sbox[256] ){
    uint8_t rByte = (uint8_t) rand();

    while(array_contains(&sbox, 256 , rByte)){
        rByte = (uint8_t) rand();
    }

    return rByte; 
}















