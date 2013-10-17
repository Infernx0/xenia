/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_KERNEL_XBOX_H_
#define XENIA_KERNEL_XBOX_H_

#include <xenia/common.h>
#include <xenia/core.h>


namespace xe {
namespace kernel {


typedef uint32_t X_HANDLE;
#define X_INVALID_HANDLE_VALUE  ((X_HANDLE)-1)


// NT_STATUS (STATUS_*)
// http://msdn.microsoft.com/en-us/library/cc704588.aspx
// Adding as needed.
typedef uint32_t X_STATUS;
#define XFAILED(s) (s & X_STATUS_UNSUCCESSFUL)
#define XSUCCEEDED(s) !XFAILED(s)
#define X_STATUS_SUCCESS                                ((uint32_t)0x00000000L)
#define X_STATUS_ABANDONED_WAIT_0                       ((uint32_t)0x00000080L)
#define X_STATUS_USER_APC                               ((uint32_t)0x000000C0L)
#define X_STATUS_ALERTED                                ((uint32_t)0x00000101L)
#define X_STATUS_TIMEOUT                                ((uint32_t)0x00000102L)
#define X_STATUS_PENDING                                ((uint32_t)0x00000103L)
#define X_STATUS_UNSUCCESSFUL                           ((uint32_t)0xC0000001L)
#define X_STATUS_NOT_IMPLEMENTED                        ((uint32_t)0xC0000002L)
#define X_STATUS_ACCESS_VIOLATION                       ((uint32_t)0xC0000005L)
#define X_STATUS_INVALID_HANDLE                         ((uint32_t)0xC0000008L)
#define X_STATUS_INVALID_PARAMETER                      ((uint32_t)0xC000000DL)
#define X_STATUS_NO_SUCH_FILE                           ((uint32_t)0xC000000FL)
#define X_STATUS_NO_MEMORY                              ((uint32_t)0xC0000017L)
#define X_STATUS_ALREADY_COMMITTED                      ((uint32_t)0xC0000021L)
#define X_STATUS_ACCESS_DENIED                          ((uint32_t)0xC0000022L)
#define X_STATUS_BUFFER_TOO_SMALL                       ((uint32_t)0xC0000023L)
#define X_STATUS_OBJECT_TYPE_MISMATCH                   ((uint32_t)0xC0000024L)
#define X_STATUS_INVALID_PAGE_PROTECTION                ((uint32_t)0xC0000045L)
#define X_STATUS_MEMORY_NOT_ALLOCATED                   ((uint32_t)0xC00000A0L)
#define X_STATUS_INVALID_PARAMETER_1                    ((uint32_t)0xC00000EFL)
#define X_STATUS_INVALID_PARAMETER_2                    ((uint32_t)0xC00000F0L)
#define X_STATUS_INVALID_PARAMETER_3                    ((uint32_t)0xC00000F1L)

// MEM_*, used by NtAllocateVirtualMemory
#define X_MEM_COMMIT              0x00001000
#define X_MEM_RESERVE             0x00002000
#define X_MEM_DECOMMIT            0x00004000
#define X_MEM_RELEASE             0x00008000
#define X_MEM_FREE                0x00010000
#define X_MEM_PRIVATE             0x00020000
#define X_MEM_RESET               0x00080000
#define X_MEM_TOP_DOWN            0x00100000
#define X_MEM_NOZERO              0x00800000
#define X_MEM_LARGE_PAGES         0x20000000
#define X_MEM_HEAP                0x40000000
#define X_MEM_16MB_PAGES          0x80000000 // from Valve SDK

// FILE_*, used by NtOpenFile
#define X_FILE_SUPERSEDED         0x00000000
#define X_FILE_OPENED             0x00000001
#define X_FILE_CREATED            0x00000002
#define X_FILE_OVERWRITTEN        0x00000003
#define X_FILE_EXISTS             0x00000004
#define X_FILE_DOES_NOT_EXIST     0x00000005


// PAGE_*, used by NtAllocateVirtualMemory
#define X_PAGE_NOACCESS           0x00000001
#define X_PAGE_READONLY           0x00000002
#define X_PAGE_READWRITE          0x00000004
#define X_PAGE_WRITECOPY          0x00000008
#define X_PAGE_EXECUTE            0x00000010
#define X_PAGE_EXECUTE_READ       0x00000020
#define X_PAGE_EXECUTE_READWRITE  0x00000040
#define X_PAGE_EXECUTE_WRITECOPY  0x00000080
#define X_PAGE_GUARD              0x00000100
#define X_PAGE_NOCACHE            0x00000200
#define X_PAGE_WRITECOMBINE       0x00000400


// (?), used by KeGetCurrentProcessType
#define X_PROCTYPE_IDLE   0
#define X_PROCTYPE_USER   1
#define X_PROCTYPE_SYSTEM 2


// Thread enums.
#define X_CREATE_SUSPENDED        0x00000004


// TLS specials.
#define X_TLS_OUT_OF_INDEXES      UINT32_MAX  // (-1)


// Languages.
#define X_LANGUAGE_ENGLISH        1
#define X_LANGUAGE_JAPANESE       2


class X_ANSI_STRING {
public:
  uint16_t  length;
  uint16_t  maximum_length;
  char*     buffer;

  X_ANSI_STRING() {
    Zero();
  }
  X_ANSI_STRING(const uint8_t* base, uint32_t p) {
    Read(base, p);
  }
  void Read(const uint8_t* base, uint32_t p) {
    length = XEGETUINT16BE(base + p);
    maximum_length = XEGETUINT16BE(base + p + 2);
    if (maximum_length) {
      buffer = (char*)(base + XEGETUINT32BE(base + p + 4));
    } else {
      buffer = 0;
    }
  }
  void Zero() {
    length = maximum_length = 0;
    buffer = 0;
  }
};


class X_OBJECT_ATTRIBUTES {
public:
  uint32_t      root_directory;
  uint32_t      object_name_ptr;
  X_ANSI_STRING object_name;
  uint32_t      attributes;

  X_OBJECT_ATTRIBUTES() {
    Zero();
  }
  X_OBJECT_ATTRIBUTES(const uint8_t* base, uint32_t p) {
    Read(base, p);
  }
  void Read(const uint8_t* base, uint32_t p) {
    root_directory  = XEGETUINT32BE(base + p);
    object_name_ptr = XEGETUINT32BE(base + p + 4);
    if (object_name_ptr) {
      object_name.Read(base, object_name_ptr);
    } else {
      object_name.Zero();
    }
    attributes      = XEGETUINT32BE(base + p + 8);
  }
  void Zero() {
    root_directory = 0;
    object_name_ptr = 0;
    object_name.Zero();
    attributes = 0;
  }
};


}  // namespace kernel
}  // namespace xe


#endif  // XENIA_KERNEL_XBOX_H_
