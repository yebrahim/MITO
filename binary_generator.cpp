#include <stdio.h>
#include <stdlib.h>

#define ArrSize 19

int main(int argc, char** argv) {
    double *iArr = (double *)malloc(8 * ArrSize);
    for (int iNum = 0; iNum < ArrSize; ++iNum)
        iArr[iNum] = iNum;

    int gigs = atoi(argv[1]);
    FILE *fp = fopen(argv[2], "w");
    int iSize = int(1024 * 1024 * gigs / (8 * ArrSize));
    for (int iCount = 0; iCount < 1024; ++iCount)
        for (int iIt = 0; iIt < iSize; ++iIt)
            fwrite((void *)iArr, 8, ArrSize, fp);
    free(iArr);
    printf("file generated..\n");
}

