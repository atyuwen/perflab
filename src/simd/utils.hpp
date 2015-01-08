#pragma once

#include "simd.hpp"

#if defined(_M_IX86)

#define SIMD_FORCE_INSTANTIATION(class_name) \
	template class_name<simd::sse_2>;        \
	template class_name<simd::sse_4_1>;      \
//  SIMD_FORCE_INSTANTIATION

#elif defined(_M_ARM)

#define SIMD_FORCE_INSTANTIATION(class_name) \
	template class_name<simd::neon>;         \
//  SIMD_FORCE_INSTANTIATION

#endif

//////////////////////////////////////////////////////////////////////////
// provide a way to access members of simd::extension.
// independent names are not visible to subclass due to two-phase name lookup,
// so we manually import all independent names here to make compiler happy.
#define SIMD_EXTENSION_PROPOGATION(iset)                        \
    typedef simd::simd_extension<iset> simd_extension;          \
    typedef typename simd_extension::packed_float packed_float; \
    typedef typename simd_extension::packed_int   packed_int;   \
    using simd_extension::arch;                                 \
    using simd_extension::pack_size;                            \
    using simd_extension::load;                                 \
    using simd_extension::load_unaligned;                       \
    using simd_extension::store;                                \
    using simd_extension::stream;                               \
    using simd_extension::set;                                  \
    using simd_extension::splat;                                \
    using simd_extension::set_zero;                             \
    using simd_extension::set_one;                              \
    using simd_extension::rcp_approx;                           \
    using simd_extension::rcp;                                  \
    using simd_extension::abs;                                  \
    using simd_extension::min;                                  \
    using simd_extension::max;                                  \
    using simd_extension::min_lane;                             \
    using simd_extension::max_lane;                             \
    using simd_extension::digest;                               \
    using simd_extension::movemask;                             \
    using simd_extension::blend;                                \
    using simd_extension::bitwise_blend;                        \
    using simd_extension::add;                                  \
    using simd_extension::sub;                                  \
    using simd_extension::mul;                                  \
    using simd_extension::muladd;                               \
    using simd_extension::div;                                  \
    using simd_extension::bitwise_and;                          \
    using simd_extension::bitwise_or;                           \
    using simd_extension::bitwise_xor;                          \
    using simd_extension::bitwise_not;                          \
    using simd_extension::bitwise_andnot;                       \
    using simd_extension::is_greater;                           \
    using simd_extension::is_greater_equal;                     \
    using simd_extension::is_less;                              \
    using simd_extension::is_less_equal;                        \
    using simd_extension::is_equal;                             \
    using simd_extension::merge_low;                            \
    using simd_extension::merge_high;                           \
    using simd_extension::merge_odd;                            \
    using simd_extension::merge_even;                           \
    using simd_extension::transpose;                            \
    using simd_extension::set_zeros;                            \
    using simd_extension::set_ones;                             \
    using simd_extension::allzeros;                             \
    using simd_extension::convert_int;                          \
    using simd_extension::convert_float;                        \
    using simd_extension::cast_int;                             \
    using simd_extension::cast_float;                           \
    using simd_extension::flushzero_on;                         \
    using simd_extension::flushzero_off;                        \
                                                                \
    template<int lane>                                          \
    static packed_float splat_lane(const packed_float& v)       \
    {                                                           \
        return simd_extension::template splat_lane<lane>(v);    \
    }                                                           \
                                                                \
    template<int lane>                                          \
    static packed_int splat_lane(const packed_int& v)           \
    {                                                           \
        return simd_extension::template splat_lane<lane>(v);    \
    }                                                           \
                                                                \
    template<int n>                                             \
    static packed_int shift_right(const packed_int &a)          \
    {                                                           \
        return simd_extension::template shift_right<n>(a);      \
    }                                                           \
                                                                \
    template<int n>                                             \
    static packed_int shift_left(const packed_int &a)           \
    {                                                           \
        return simd_extension::template shift_left<n>(a);       \
    }                                                           \
                                                                \
    template<int hint>                                          \
    static void prefetch(const char* address)                   \
    {                                                           \
        simd_extension::template prefetch<hint>(address);       \
    }                                                           \
//  SIMD_EXTENSION_PROPOGATION
