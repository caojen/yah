#include "cjson_array.h"

cjson_array*
cjson_array_init() {
  cjson_array* ret = (cjson_array*) malloc (sizeof(cjson_array));
  memset(ret, 0, sizeof(cjson_array));
  ret->type = ARRAY;
  ret->list = cjson_list_init();
  return ret;
}

int
cjson_array_push(cjson_array* array, void* value) {
  if(array == NULL) {
    return CJSON_UNINIT;
  } else if(value == NULL) {
    value = cjson_null_create();
  }

  return cjson_list_push_back(array->list, value);
}

void* cjson_array_at(const cjson_array* array, unsigned index) {
  if(index >= array->list->length) {
    return NULL;
  }

  cjson_list_node* listnode = array->list->head;
  while(index--) {
    listnode = listnode->next;
  }
  return listnode->item;
}

int
cjson_array_free(cjson_array* array) {
  if(array) {
    cjson_list_free(array->list);
    free(array);
  }
  return CJSON_OK;
}

int
cjson_array_free_whole(cjson_array* array) {
  if(array) {
    cjson_list_node* listnode = array->list->head;
    while(listnode) {
      cjson_type_free(listnode->item);
      listnode = listnode->next;
    }
    cjson_array_free(array);
  }
  return CJSON_OK;
}

int
cjson_array_stringify(const cjson_array* array, unsigned char* buf, unsigned maxsz) {
  if(array == NULL) {
    return CJSON_UNINIT;
  } else if(buf == NULL) {
    return CJSON_BUFCLOSE;
  }
  unsigned char* ptr = buf;
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = '['; ++ptr;

  if(array->list->length != 0) {
    cjson_list_node* listnode = array->list->head;
    while(listnode) {
      if(listnode != array->list->head) {
        CHECK_NO_SPACE(ptr - buf >= maxsz);
        *ptr = ','; ++ptr;
      }
      void* value = listnode->item;
      unsigned used = ptr - buf;
      unsigned remain = maxsz - used;
      int res = cjson_type_stringify(value, ptr, remain);
      if(res != CJSON_OK) {
        return res;
      }
      while(*ptr) {
        ++ptr;
      }
      listnode = listnode->next;
    }
  }

  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = ']'; ++ptr;
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = 0;
  return CJSON_OK;
}

unsigned cjson_array_length(cjson_array* array) {
  unsigned ret = 0;
  if(array) {
    ret = array->list->length;
  }
  return ret;
}
