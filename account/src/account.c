#include "account.h"

Account *accounts[0x10];

uint8_t add_ref(uint8_t account_id) {
  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  ac->ref_cnt++;

  return ac->ref_cnt;
}

uint8_t release(uint8_t account_id) {
  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  ac->ref_cnt--;

  if (ac->ref_cnt == 0) {
    mm_free(ac->name.ascii);
    mm_free(ac);
  }

  return ac->ref_cnt;
}

uint8_t new_account(uint8_t type, void *name) {
  for (int i = 0; i < 16; i++) {
    if (accounts[i] == NULL) {
      uint32_t name_size;

      switch (type) {
      case ACCOUNT_TYPE_ASCII:
        if ((name_size = strlen((char *)name)) == 0) {
          fprintf(stderr, "invalid length\n");
          return -1;
        }
        // 1 for null byte
        name_size = name_size + 1;
        break;
      case ACCOUNT_TYPE_UNICODE:
        if ((name_size = utf16_strlen((Utf16 *)name)) == 0) {
          fprintf(stderr, "invalid length\n");
          return -1;
        }
        // 1 for null byte
        // 2 for unicode
        name_size = (name_size + 1) * 2;
        break;
      default:
        fprintf(stderr, "invalid account type\n");
        return -1;
      }

      Account *account = (Account *)mm_alloc(sizeof(Account));
      void *name_buf = mm_alloc(name_size);

      if (name_buf == NULL || account == NULL) {
        fprintf(stderr, "failed to allocate memory\n");
        return -1;
      }

      account->type = type;
      account->ref_cnt = 0;
      account->name.ascii = name_buf;

      switch (type) {
      case ACCOUNT_TYPE_ASCII:
        strcpy(name_buf, (char *)name);
        break;
      case ACCOUNT_TYPE_UNICODE:
        utf16_strcpy(name_buf, (Utf16 *)name);
        break;
      default:
        fprintf(stderr, "invalid account type\n");
        return -1;
      }

      accounts[i] = account;
      add_ref(i);
      return i;
    }
  }

  fprintf(stderr, "no more account\n");
  return -1;
}

int del_account(uint8_t account_id) {
  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  if (ac->ref_cnt != 1) {
    fprintf(stderr, "account is still in use\n");
    return -1;
  }

  if (release(account_id) != 0) {
    fprintf(stderr, "unexpected flow\n");
    return -1;
  }

  accounts[account_id] = NULL;

  return 0;
}

int _show_account(Account *ac) {
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  switch (ac->type) {
  case ACCOUNT_TYPE_ASCII:
    for (int i = 0; ac->name.ascii->string[i]; i++) {
      printf("%c", ac->name.ascii->string[i]);
    }
    break;
  case ACCOUNT_TYPE_UNICODE:
    for (int i = 0; ac->name.ascii->string[i] || ac->name.ascii->string[i + 1];
         i += 2) {
      printf("%c%c", ac->name.ascii->string[i], ac->name.ascii->string[i + 1]);
    }
    break;
  default:
    fprintf(stderr, "unexpected\n");
    return -1;
  }

  return 0;
}

int show_account(uint8_t account_id) {
  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  return _show_account(ac);
}

int fix_account(uint8_t type, uint8_t account_id, void *name) {
  if (account_id >= 0x10) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  Account *ac = accounts[account_id];
  if (!ac) {
    fprintf(stderr, "invalid account id\n");
    return -1;
  }

  uint32_t allocated_size;
  switch (ac->type) {
  case ACCOUNT_TYPE_ASCII:
    allocated_size = strlen(ac->name.ascii->string) + 1;
    break;
  case ACCOUNT_TYPE_UNICODE:
    allocated_size = (utf16_strlen(ac->name.unicode->utf16_string) + 1) * 2;
    break;
  default:
    fprintf(stderr, "unexpected\n");
    return -1;
  }

  switch (type) {
  case ACCOUNT_TYPE_ASCII:
    // allocated_size는 NULL을 포함한 길이
    // 길이 비교를 할 떈, NULL을 포함하지 않은 길이를 비교함
    // 하지만, Ascii는 뒤에 NULL 문자가 하나만 붙어서 문제가 발생하지 않음
    if (strlen((char *)name) >= allocated_size) {
      fprintf(stderr, "invalid length\n");
      return -1;
    }
    strcpy(ac->name.ascii->string, (char *)name);
    ac->type = ACCOUNT_TYPE_ASCII;
    break;
  case ACCOUNT_TYPE_UNICODE:
    // Unicode는 뒤에 NULL 문자가 두 개 붙음
    // allocated_size-1 한 길이가 들어오면 Off-by-one이 발생
    if (utf16_strlen((Utf16 *)name) * 2 >= allocated_size) {
      fprintf(stderr, "invalid length\n");
      return -1;
    }
    utf16_strcpy(ac->name.unicode->utf16_string, (Utf16 *)name);
    ac->type = ACCOUNT_TYPE_UNICODE;
    break;
  default:
    fprintf(stderr, "unexpected\n");
    return -1;
  }

  _show_account(ac);
  return 0;
}
