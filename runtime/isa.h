/*
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 2018 Apple Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/********************************************************************
 * 
 *  isa.h - Definitions of isa fields for C and assembly code.
 *
 ********************************************************************/

#ifndef _OBJC_ISA_H_
#define _OBJC_ISA_H_

#include "objc-config.h"


#if (!SUPPORT_NONPOINTER_ISA && !SUPPORT_PACKED_ISA && !SUPPORT_INDEXED_ISA) ||\
    ( SUPPORT_NONPOINTER_ISA &&  SUPPORT_PACKED_ISA && !SUPPORT_INDEXED_ISA) ||\
    ( SUPPORT_NONPOINTER_ISA && !SUPPORT_PACKED_ISA &&  SUPPORT_INDEXED_ISA)
    // good config
#else
#   error bad config
#endif


#if SUPPORT_PACKED_ISA

    // extra_rc must be the MSB-most field (so it matches carry/overflow flags)
    // nonpointer must be the LSB (fixme or get rid of it)
    // shiftcls must occupy the same bits that a real class pointer would
    // bits + RC_ONE is equivalent to extra_rc + 1
    // RC_HALF is the high bit of extra_rc (i.e. half of its range)

    // future expansion:
    // uintptr_t fast_rr : 1;     // no r/r overrides
    // uintptr_t lock : 2;        // lock for atomic property, @synch
    // uintptr_t extraBytes : 1;  // allocated with extra bytes

/*
 isa_t 的 成员说明：
 nonpointer:
    0: 代表普通的指针，存储着Class和Meta Class 对象的内存地址
    1: 代表优化过的isa，使用位域存储更多信息
 has_assoc：
    是否有设置关联属性（注意只要曾经设置过），如果没有释放的更快
 has_cxx_dtor:
    是否有c++的析构函数(.cxx_destruct)，如果没有释放的更快
 shiftcls:
    当nonpointer为1时，存储着Class 和 Meta Class 对象的内存地址
 magic:
    用于在调试时，分辨对象释放未完成初始化
 weakly_referenced:
    是否被弱引用指向过，如果没有释放的更快
 deallocating:
    对象是否正在释放
 has_sidetable_rc:
    0: 当引用计数较小时，存储在extra_rc中，此时has_sidetable_rc为0
    1: 当引用计数过大时，无法存储在isa（extra_rc）中，此时才使用SideTable存储额外的引用计数，has_sidetable_rc为1
uintptr_t extra_rc          : 8
 */

# if __arm64__
#   define ISA_MASK        0x0000000ffffffff8ULL
#   define ISA_MAGIC_MASK  0x000003f000000001ULL
#   define ISA_MAGIC_VALUE 0x000001a000000001ULL
#   define ISA_BITFIELD                                                      \
      uintptr_t nonpointer        : 1;                                       \
      uintptr_t has_assoc         : 1;                                       \
      uintptr_t has_cxx_dtor      : 1;                                       \
      uintptr_t shiftcls          : 33; /*MACH_VM_MAX_ADDRESS 0x1000000000*/ \
      uintptr_t magic             : 6;                                       \
      uintptr_t weakly_referenced : 1;                                       \
      uintptr_t deallocating      : 1;                                       \
      uintptr_t has_sidetable_rc  : 1;                                       \
      uintptr_t extra_rc          : 19
#   define RC_ONE   (1ULL<<45)
#   define RC_HALF  (1ULL<<18)

# elif __x86_64__
#   define ISA_MASK        0x00007ffffffffff8ULL
#   define ISA_MAGIC_MASK  0x001f800000000001ULL
#   define ISA_MAGIC_VALUE 0x001d800000000001ULL
#   define ISA_BITFIELD                                                        \
      uintptr_t nonpointer        : 1;                                         \
      uintptr_t has_assoc         : 1;                                         \
      uintptr_t has_cxx_dtor      : 1;                                         \
      uintptr_t shiftcls          : 44; /*MACH_VM_MAX_ADDRESS 0x7fffffe00000*/ \
      uintptr_t magic             : 6;                                         \
      uintptr_t weakly_referenced : 1;                                         \
      uintptr_t deallocating      : 1;                                         \
      uintptr_t has_sidetable_rc  : 1;                                         \
      uintptr_t extra_rc          : 8
#   define RC_ONE   (1ULL<<56)
#   define RC_HALF  (1ULL<<7)

# else
#   error unknown architecture for packed isa
# endif

// SUPPORT_PACKED_ISA
#endif


#if SUPPORT_INDEXED_ISA

# if  __ARM_ARCH_7K__ >= 2  ||  (__arm64__ && !__LP64__)
    // armv7k or arm64_32

#   define ISA_INDEX_IS_NPI_BIT  0
#   define ISA_INDEX_IS_NPI_MASK 0x00000001
#   define ISA_INDEX_MASK        0x0001FFFC
#   define ISA_INDEX_SHIFT       2
#   define ISA_INDEX_BITS        15
#   define ISA_INDEX_COUNT       (1 << ISA_INDEX_BITS)
#   define ISA_INDEX_MAGIC_MASK  0x001E0001
#   define ISA_INDEX_MAGIC_VALUE 0x001C0001
#   define ISA_BITFIELD                         \
      uintptr_t nonpointer        : 1;          \
      uintptr_t has_assoc         : 1;          \
      uintptr_t indexcls          : 15;         \
      uintptr_t magic             : 4;          \
      uintptr_t has_cxx_dtor      : 1;          \
      uintptr_t weakly_referenced : 1;          \
      uintptr_t deallocating      : 1;          \
      uintptr_t has_sidetable_rc  : 1;          \
      uintptr_t extra_rc          : 7
#   define RC_ONE   (1ULL<<25)
#   define RC_HALF  (1ULL<<6)

# else
#   error unknown architecture for indexed isa
# endif

// SUPPORT_INDEXED_ISA
#endif


// _OBJC_ISA_H_
#endif
