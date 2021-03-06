/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Trace macros with masks and levels
///
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <libgen.h>
#include <stdarg.h>

/// Add trace groups below - must be powers of two
// clang-format off
const unsigned int TRC_G_TIME   = 0x00000001U;
const unsigned int TRC_G_DEBUG  = 0x00000002U;
const unsigned int TRC_G_STATS  = 0x00000004U;
const unsigned int TRC_G_MAIN   = 0x00000008U;
const unsigned int TRC_G_DIGIT  = 0x00000010U;
const unsigned int TRC_G_EVENT  = 0x00000020U;
const unsigned int TRC_G_DATAH  = 0x00000040U;
const unsigned int TRC_G_UDP    = 0x00000080U;
const unsigned int TRC_G_CONF   = 0x00000100U; // Configuration

/// Add trace masks below, bitwise or of grouops

// Do not edit below
const unsigned int TRC_M_NONE = 0;
const unsigned int TRC_M_ALL  = 0xffffffffU;


// Trace levels
const unsigned int TRC_L_ALW  = 1; //Should not be used
const unsigned int TRC_L_CRI  = 2;
const unsigned int TRC_L_ERR  = 3;
const unsigned int TRC_L_WAR  = 4;
const unsigned int TRC_L_NOTE = 5;
const unsigned int TRC_L_INF  = 6;
const unsigned int TRC_L_DEB  = 7;
// clang-format on

/// \brief get rid of annoying warning
/// \todo See if there is a better solution than pragma
#pragma GCC system_header

#define TRC_MASK TRC_M_ALL
//#define TRC_MASK TRC_G_EVENT
#define TRC_LEVEL TRC_L_WAR


inline int Trace(int const LineNumber, char const *File, const char* GroupName,  const char* SeverityName, const char *Format, ...) {
  char *MessageBuffer = nullptr;

  va_list args;
  va_start (args, Format);
  vasprintf(&MessageBuffer, Format, args);
  va_end (args);

  printf("%-4s %-20s %5d %-7s - %s\n", SeverityName, basename((char *)File), LineNumber, GroupName, MessageBuffer);

  free(MessageBuffer);
  return 0;
}

#define XTRACE(Group, Level, Format, ...) \
   (void) ( ((TRC_L_##Level <= TRC_LEVEL) && (TRC_MASK & TRC_G_##Group)) \
   ? Trace(__LINE__, __FILE__, #Group, #Level, Format, ##__VA_ARGS__) \
   : 0)
