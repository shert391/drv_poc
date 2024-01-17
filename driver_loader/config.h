#pragma once

#define RTCORE_NAME L"RTCore64.sys"

#define SIG_CI_OPTIONS "\xF7\x05\x00\x00\x00\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x41\x0F\xBA\xEF" // .PAGE:CiAuditImageForHvci + 0x146: 
																										  // Win10 22H2 Build 19045 and Win11 23H2 Build 22631