#pragma once

#include <stdint.h>
#include <stdio.h>

#include "account.h"
#include "mm.h"

typedef struct _GroupVTable {
  uint8_t (*new_group)();
  int (*del_group)(uint8_t group_id);
  uint8_t (*add_group)(uint8_t group_id, uint8_t account_id);
  uint8_t (*sub_group)(uint8_t group_id, uint8_t account_id);
  int (*list_group)(uint8_t group_id);
} GroupVTable;

typedef struct _Group {
  uint32_t count;
  Account **members;
  GroupVTable *vtable;
} Group;

extern Group *groups[0x10];

uint8_t new_group();
int del_group(uint8_t group_id);
uint8_t add_group(uint8_t group_id, uint8_t account_id);
uint8_t sub_group(uint8_t group_id, uint8_t account_id);
int list_group(uint8_t group_id);