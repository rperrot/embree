// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../sys/platform.h"

namespace embree
{
  /* Varying numeric types */
  template<int N>
  struct vfloat
  {
    union { float f[N]; int i[N]; };
    __forceinline const float& operator [](size_t index) const { assert(index < N); return f[index]; }
    __forceinline       float& operator [](size_t index)       { assert(index < N); return f[index]; }
  };

  template<int N>
  struct vdouble
  {
    union { double f[N]; int64_t i[N]; };
    __forceinline const double& operator [](size_t index) const { assert(index < N); return f[index]; }
    __forceinline       double& operator [](size_t index)       { assert(index < N); return f[index]; }
  };

  template<int N>
  struct vint
  {
    int i[N];
    __forceinline const int& operator [](size_t index) const { assert(index < N); return i[index]; }
    __forceinline       int& operator [](size_t index)       { assert(index < N); return i[index]; }
  };

  template<int N>
  struct vuint
  {
    unsigned int i[N];
    __forceinline const unsigned int& operator [](size_t index) const { assert(index < N); return i[index]; }
    __forceinline       unsigned int& operator [](size_t index)       { assert(index < N); return i[index]; }
  };

  template<int N>
  struct vlong
  {
    int64_t i[N];
    __forceinline const int64_t& operator [](size_t index) const { assert(index < N); return i[index]; }
    __forceinline       int64_t& operator [](size_t index)       { assert(index < N); return i[index]; }
  };

#if !defined(_MSC_VER) || _MSC_VER >= 1800
  /* Varying bool types */
  template<int N> struct vboolf { int     i[N]; }; // for float/int
  template<int N> struct vboold { int64_t i[N]; }; // for double/long

  /* Aliases to default types */
  template<int N> using vreal = vfloat<N>;
  template<int N> using vbool = vboolf<N>;
#else
  /* Workaround for VS2012 */
  #define vreal  vfloat
  #define vboolf vbool

  template<int N> struct vboolf { int     i[N]; };
  template<int N> struct vboold { int64_t i[N]; };
#endif

  /* Maximum supported varying size */
#if defined(__AVX512F__)
  const int VSIZEX = 16;
#elif defined(__AVX__)
  const int VSIZEX = 8;
#else
  const int VSIZEX = 4;
#endif

  /* Extends varying size N to optimal or up to max(N, N2) */
  template<int N, int N2 = VSIZEX>
  struct vextend
  {
#if defined(__AVX512F__) && !defined(__AVX512VL__) // KNL
    static const int size = (N2 == VSIZEX) ? VSIZEX : N;
#else
    static const int size = N;
#endif
  };

  /* 4-wide shortcuts */
  typedef vfloat<4>  vfloat4;
  typedef vdouble<4> vdouble4;
  typedef vreal<4>   vreal4;
  typedef vint<4>    vint4;
  typedef vlong<4>   vlong4;
  typedef vbool<4>   vbool4;
  typedef vboolf<4>  vboolf4;
  typedef vboold<4>  vboold4;

  /* 8-wide shortcuts */
  typedef vfloat<8>  vfloat8;
  typedef vdouble<8> vdouble8;
  typedef vreal<8>   vreal8;
  typedef vint<8>    vint8;
  typedef vlong<8>   vlong8;
  typedef vbool<8>   vbool8;
  typedef vboolf<8>  vboolf8;
  typedef vboold<8>  vboold8;

  /* 16-wide shortcuts */
  typedef vfloat<16>  vfloat16;
  typedef vdouble<16> vdouble16;
  typedef vreal<16>   vreal16;
  typedef vint<16>    vint16;
  typedef vuint<16>   vuint16;
  typedef vlong<16>   vlong16;
  typedef vbool<16>   vbool16;
  typedef vboolf<16>  vboolf16;
  typedef vboold<16>  vboold16;

  /* Maximum shortcuts */
  typedef vfloat<VSIZEX>  vfloatx;
  typedef vdouble<VSIZEX> vdoublex;
  typedef vreal<VSIZEX>   vrealx;
  typedef vint<VSIZEX>    vintx;
  typedef vlong<VSIZEX>   vlongx;
  typedef vbool<VSIZEX>   vboolx;
  typedef vboolf<VSIZEX>  vboolfx;
  typedef vboold<VSIZEX>  vbooldx;
}
