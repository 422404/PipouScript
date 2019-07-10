#include <assert.h>
#include <stdio.h>
#include "Common/include/vector.h"

void Test_VectorCreation(void) {
    vector_t * vector;

    vector = Vec_New();
    assert(vector != NULL);
    printf("Test_VectorCreation(): OK\n");
}
