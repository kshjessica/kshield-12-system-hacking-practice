#pragma once

#include <stdint.h>
#include <stdio.h>

#include "account.h"
#include "group.h"

#define CALL_NUM_NEW_ACCOUNT (0x00)
#define CALL_NUM_DEL_ACCOUNT (0x01)
#define CALL_NUM_FIX_ACCOUNT (0x02)
#define CALL_NUM_NEW_GROUP (0x10)
#define CALL_NUM_DEL_GROUP (0x11)
#define CALL_NUM_ADD_GROUP (0x12)
#define CALL_NUM_SUB_GROUP (0x13)
#define CALL_NUM_LIST_GROUP (0x14)

typedef struct __attribute__((packed)) _NewAccount {
  uint8_t type;
  unsigned char name[1];
} NewAccount;

typedef struct __attribute__((packed)) _DelAccount {
  uint8_t account_id;
} DelAccount;

typedef struct __attribute__((packed)) _FixAccount {
  uint8_t account_id;
  uint8_t type;
  unsigned char name[1];
} FixAccount;

typedef struct __attribute__((packed)) _DelGroup {
  uint8_t group_id;
} DelGroup;

typedef struct __attribute__((packed)) _AddGroup {
  uint8_t group_id;
  uint8_t account_id;
} AddGroup;

typedef struct __attribute__((packed)) _SubGroup {
  uint8_t group_id;
  uint8_t account_id;
} SubGroup;

typedef struct __attribute__((packed)) _ListGroup {
  uint8_t group_id;
} ListGroup;

typedef struct __attribute__((packed)) _Packet {
  uint8_t call_num;
  union {
    NewAccount new_account;
    DelAccount del_account;
    FixAccount fix_account;

    DelGroup del_group;
    AddGroup add_group;
    SubGroup sub_group;
    ListGroup list_group;
  } data;
} Packet;

int process_packet(unsigned char *buf, uint32_t len);
