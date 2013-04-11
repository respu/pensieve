#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>	
#include <immintrin.h>
#include <math.h>
#include <bam/timer.hpp>
#include <iostream>
#include <algorithm>
#include <numeric>

int main()
{
#ifdef USE_AVX
    const int alignment = 32;
#else
    const int alignment = 16;
#endif
    const int size = 160000;
    void* pootr;
    if(!posix_memalign(&pootr, alignment, size * sizeof(float))) {};
    float* floating_pootr = (float*)pootr;


    for(auto i = 0; i != size; ++i) {
      floating_pootr[i] = 1;
    }

    bam::basic_timer timer;

    for(int lamerz = 0; lamerz != 10000; ++lamerz) {
#ifdef USE_SSE 
        const int vec_size = alignment / 4;
        __m128* sse_pootr = (__m128*) floating_pootr;

        for(auto i = 0; i != size/vec_size; ++i) {
            const __m128 to_be_added = _mm_set_ps(2.0f, 2.0f, 2.0f, 2.0f);

            sse_pootr[i] = _mm_add_ps(sse_pootr[i], to_be_added);
        }

        
#elif defined USE_AVX

        const int vec_size = alignment / 4;
        __m256* avx_pootr = (__m256*)floating_pootr;
      
        for(auto i = 0; i != size/vec_size; ++i) {
          const __m256 to_be_added = _mm256_set_ps(2.0f, 2.0f, 2.0f,2.0f,2.0f, 2.0f, 2.0f,2.0f);
      
          avx_pootr[i] = _mm256_add_ps(avx_pootr[i], to_be_added);
        }
      
#else // auto vectorization
        std::for_each(floating_pootr, floating_pootr + size, [] (float& x) { x += 2.0f; });
#endif
    } // repeat 
    std::cout << "elapsed " << timer.elapsed() << std::endl;
    std::cout << "checksum " << std::accumulate(floating_pootr, floating_pootr + size, 0.0f) << std::endl;
  

	return 0;
}
