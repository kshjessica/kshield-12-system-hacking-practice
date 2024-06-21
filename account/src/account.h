#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mm.h"
#include "utf16.h"

#define ACCOUNT_TYPE_UNICODE (0)
#define ACCOUNT_TYPE_ASCII (1)

typedef struct _Ascii {
  char string[1];
} Ascii;

typedef struct _Unicode {
  Utf16 utf16_string[1];
} Unicode;

typedef struct _Account {
  uint8_t type;
  uint8_t ref_cnt;
  union {
    Ascii *ascii;
    Unicode *unicode;
  } name;
  uint64_t reserved; // ...?
} Account;

extern Account *accounts[0x10];

uint8_t add_ref(uint8_t account_id);
uint8_t release(uint8_t account_id);

uint8_t new_account(uint8_t type, void *name);
int del_account(uint8_t account_id);
int _show_account(Account *ac);
int show_account(uint8_t account_id);
int fix_account(uint8_t type, uint8_t account_id, void *name);