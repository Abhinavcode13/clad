// RUN: %cladclang %s -I%S/../../include -otestUtility.out 2>&1 | %filecheck %s
// RUN: ./testUtility.out | %filecheck_exec %s
// RUN: %cladclang -Xclang -plugin-arg-clad -Xclang -enable-tbr %s -I%S/../../include -otestUtility.out
// RUN: ./testUtility.out | %filecheck_exec %s
// CHECK-NOT: {{.*error|warning|note:.*}}

#include "clad/Differentiator/Differentiator.h"

#include "../TestUtils.h"

  double output[3] = {0};
  double jacobian[6] = {0};
  double output1[3] = {0};
  double jacobian1[9] = {0};

void fn_mul(double i, double j, double *res) {
   res[0] = i*i;
   res[1] = j*j;
   res[2] = i*j;
}

//CHECK: void fn_mul_jac(double i, double j, double *res, double *jacobianMatrix) {
//CHECK-NEXT:    res[0] = i * i;
//CHECK-NEXT:    res[1] = j * j;
//CHECK-NEXT:    res[2] = i * j;
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{4U|4UL|4ULL}}] += 1 * j;
//CHECK-NEXT:        jacobianMatrix[{{5U|5UL|5ULL}}] += i * 1;
//CHECK-NEXT:    }
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += 1 * j;
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += j * 1;
//CHECK-NEXT:    }
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{0U|0UL|0ULL}}] += 1 * i;
//CHECK-NEXT:        jacobianMatrix[{{0U|0UL|0ULL}}] += i * 1;
//CHECK-NEXT:    }
//CHECK-NEXT:}


void f_1(double x, double y, double z, double output[]) {
  output[0] = x * x * x;
  output[1] = x * y * x + y * x * x;
  output[2] = z * x * 10 - y * z;
}

//CHECK: void f_1_jac(double x, double y, double z, double output[], double *jacobianMatrix) {
//CHECK-NEXT:    output[0] = x * x * x;
//CHECK-NEXT:    output[1] = x * y * x + y * x * x;
//CHECK-NEXT:    output[2] = z * x * 10 - y * z;
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{8U|8UL|8ULL}}] += 1 * 10 * x;
//CHECK-NEXT:        jacobianMatrix[{{6U|6UL|6ULL}}] += z * 1 * 10;
//CHECK-NEXT:        jacobianMatrix[{{7U|7UL|7ULL}}] += -1 * z;
//CHECK-NEXT:        jacobianMatrix[{{8U|8UL|8ULL}}] += y * -1;
//CHECK-NEXT:    }
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += 1 * x * y;
//CHECK-NEXT:        jacobianMatrix[{{4U|4UL|4ULL}}] += x * 1 * x;
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += x * y * 1;
//CHECK-NEXT:        jacobianMatrix[{{4U|4UL|4ULL}}] += 1 * x * x;
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += y * 1 * x;
//CHECK-NEXT:        jacobianMatrix[{{3U|3UL|3ULL}}] += y * x * 1;
//CHECK-NEXT:    }
//CHECK-NEXT:    {
//CHECK-NEXT:        jacobianMatrix[{{0U|0UL|0ULL}}] += 1 * x * x;
//CHECK-NEXT:        jacobianMatrix[{{0U|0UL|0ULL}}] += x * 1 * x;
//CHECK-NEXT:        jacobianMatrix[{{0U|0UL|0ULL}}] += x * x * 1;
//CHECK-NEXT:    }
//CHECK-NEXT:}

int main(){
    INIT_JACOBIAN(fn_mul);
    INIT_JACOBIAN(f_1);

    TEST_JACOBIAN(fn_mul, 2, 6, 3, 1, output, jacobian); // CHECK-EXEC: {6.00, 0.00, 0.00, 2.00, 1.00, 3.00}
    TEST_JACOBIAN(f_1, 3, 9, 4, 5, 6, output1, jacobian1); // CHECK-EXEC: {48.00, 0.00, 0.00, 80.00, 32.00, 0.00, 60.00, -6.00, 35.00}

}

