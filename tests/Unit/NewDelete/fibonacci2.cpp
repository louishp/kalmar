// XFAIL: Linux
// RUN: %amp_device -D__GPU__ -Xclang -fhsa-ext %s -m32 -emit-llvm -c -S -O2 -o %t.ll && mkdir -p %t
// RUN: %clamp-device %t.ll %t/kernel.cl
// RUN: pushd %t && %embed_kernel kernel.cl %t/kernel.o && popd
// RUN: %cxxamp -Xclang -fhsa-ext %link %t/kernel.o %s -o %t.out && %t.out
#include <amp.h>

#define TEST_DEBUG 0

using namespace concurrency;

const int size = 16;

int main()
{
  int sumCPP[size];

  // Create C++ AMP objects.
  array_view<int, 1> sum(size, sumCPP);

  parallel_for_each(
    // Define the compute domain, which is the set of threads that are created.
    sum.get_extent(),
    // Define the code to run on each thread on the accelerator.
    [=](index<1> idx) restrict(amp)
  {
    int tid = idx[0];
    int *fib = new int[(tid < 2) ? 2 : tid + 1];

    fib[0] = 0;
    fib[1] = 1;

    for (int i = 2; i <= tid; ++i)
    {
      fib[i] = fib[i-1] + fib[i-2];
    }
 
    sum[idx] = fib[tid];
    delete[] fib;
  }
  );

#if TEST_DEBUG
  for (int i = 0; i < size; ++i)
  {
    printf("Fib[n] is %d\n", sum[i]);
  }
#endif

  int *fibh = new int[size + 1];

  fibh[0] = 0;
  fibh[1] = 1;

  for (int i = 2; i < size; i++)
  {
    fibh[i] = fibh[i-1] + fibh[i-2];
  }

  for (int i = 0; i < size; i++)
  {
    if (fibh[i] != sum[i]) {
      std::cout << "Verify failed!\n";
      return 1;
    }
  }

  delete[] fibh;

  std::cout << "Verify success!\n";
  return 0;
}