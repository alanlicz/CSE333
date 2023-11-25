#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void DumpBytes(void* pData, int32_t byteLen) {
    uint8_t* pBytes = (uint8_t*) pData;
    printf("The %d bytes starting at address %p are:", byteLen, pBytes);
    for (int32_t i = 0; i < byteLen; i++) {
        printf(" %02x", pBytes[i]); //print each byte as 2 digit lowercase hex
    }
    printf("\n");
}

void CopyAndSort(uint8_t* pSrc, uint8_t* pDest, int32_t length) {
    DumpBytes(pSrc, sizeof(pSrc));
    for (int32_t i = 0; i < length; i++) {
        int32_t j = i;
        while (j > 0 && pDest[j-1] > pSrc[i]) {  //perform insertion sort
            pDest[j] = pDest[j-1];
            j--;
        }
        pDest[j] = pSrc[i];
    }
}

int main(int argc, char* argv[]) {
  int32_t int_val = 1;
  float   float_val = 1.0f;
  uint8_t arr_unsorted[] = {3, 2, 0, 8, 17, 6, 10, 7, 8, 1, 12};
  uint8_t arr_sorted[]   = {0, 0, 0, 0,  0, 0,  0, 0, 0, 0,  0};

  DumpBytes(&int_val, sizeof(int_val));
  DumpBytes(&float_val, sizeof(float_val));
  DumpBytes(arr_unsorted, sizeof(arr_unsorted));
  CopyAndSort(arr_unsorted, arr_sorted, sizeof(arr_unsorted)/sizeof(uint8_t));
  DumpBytes(arr_sorted, sizeof(arr_sorted));

  return EXIT_SUCCESS;
}