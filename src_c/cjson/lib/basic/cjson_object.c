#include "cjson_object.h"

cjson_object_item*
cjson_object_item_create(cjson_string* key, void* value) {
  cjson_object_item* ret = (cjson_object_item*) malloc (sizeof(cjson_object_item));
  ret->key = key;
  ret->value = value;
  return ret;
}

int
cjson_object_item_free(cjson_object_item* item) {
  if(item) {
    free(item);
  }
  return CJSON_OK;
}

cjson_object*
cjson_object_init() {
  cjson_object* ret = (cjson_object*) malloc (sizeof(cjson_object));
  memset(ret, 0, sizeof(cjson_object));
  ret->type = OBJECT;
  ret->list = cjson_list_init();
  return ret;
}

int cjson_object_set(cjson_object* object, cjson_string* key, void* value) {
  if(object == NULL) {
    return CJSON_UNINIT;
  } else if(key == NULL) {
    return CJSON_NKEY;
  } else if(cjson_get_type(object) != OBJECT || cjson_get_type(key) != STRING) {
    return CJSON_ERRTYPE;
  }

  cjson_list_node* listnode = object->list->head;
  while(listnode) {
    cjson_object_item* item = (cjson_object_item*)listnode->item;
    if(cjson_string_cmp(key, item->key) == 0) {
      // same, just delete the old value and set the new value
      cjson_type_free(item->value);
      item->value = value;
      return CJSON_OK;
    }
    listnode = listnode->next;
  }

  // not exists key, just push
  cjson_object_item* item = cjson_object_item_create(key, value);
  cjson_list_push_back(object->list, item);
  return CJSON_OK;
}

int
cjson_object_del(cjson_object* object, cjson_string* key) {
  if(object == NULL) {
    return CJSON_UNINIT;
  } else if(key == NULL) {
    return CJSON_NKEY;
  } else if(cjson_get_type(object) != OBJECT || cjson_get_type(key) != STRING) {
    return CJSON_ERRTYPE;
  }

  cjson_list_node* listnode = object->list->head;
  while(listnode) {
    cjson_object_item* item = (cjson_object_item*)listnode->item;
    if(cjson_string_cmp(item->key, key) == 0) {
      // found, delete this key and value
      cjson_string_free(item->key);
      cjson_type_free(item->value);
      cjson_list_del(object->list, listnode);
      break;
    }
    listnode = listnode->next;
  }
  return CJSON_OK;
}

int
cjson_object_free(cjson_object* object) {
  if(object) {
    cjson_list_free(object->list);
    free(object);
  }

  return CJSON_OK;
}

int
cjson_object_free_whole(cjson_object* object) {
  if(object) {
    cjson_list_node* listnode = object->list->head;
    while(listnode) {
      cjson_object_item* item = (cjson_object_item*)listnode->item;
      cjson_string_free(item->key);
      cjson_type_free(item->value);
      cjson_object_item_free(item);
      listnode = listnode->next;
    }

    cjson_object_free(object);
  }
  return CJSON_OK;
}

int
cjson_object_stringify(cjson_object* object, unsigned char* buf, unsigned maxsz) {
  if(object == NULL) {
    return CJSON_UNINIT;
  } else if(buf == NULL) {
    return CJSON_BUFCLOSE;
  }

  int ret = CJSON_OK;

  unsigned char* ptr = buf;
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = '{'; ++ptr;
  
  if(object->list->length != 0) {
    cjson_list_node* listnode = object->list->head;
    while(listnode) {
      if(listnode != object->list->head) {
        CHECK_NO_SPACE(ptr - buf >= maxsz);
        *ptr = ','; ++ptr;
      }
      unsigned used = ptr - buf;
      unsigned remain = maxsz - used;

      cjson_object_item* item = (cjson_object_item*) listnode->item;
      ret = cjson_string_stringify(item->key, ptr, remain);
      if(ret != CJSON_OK) {
        return ret;
      }
      while(*ptr) ++ptr;

      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = ':'; ++ptr;
      used = ptr - buf;
      remain = maxsz - used;
      ret = cjson_type_stringify(item->value, ptr, remain);
      if(ret != CJSON_OK) {
        return ret;
      }
      while(*ptr) ++ptr;

      listnode = listnode->next;
    }
  }
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = '}'; ++ptr;
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = 0;
  return ret;
}
