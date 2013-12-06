#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "device_database.h"
#include "device_address.c"
 
#define SQL_CREATE_SUPPORTED_DEVICES \
  "create table supported_devices(" \
    "device_id integer primary key asc, " \
    "device text not null, " \
    "build_id text not null, " \
    "check_property_name text, " \
    "check_property_value text, " \
    "unique (device, build_id, check_property_name, check_property_value));" \

#define SQL_CREATE_DEVICES_ADDRESS \
  "create table device_address(" \
    "device_id integer not null, " \
    "name text not null, " \
    "value integer not null, " \
    "primary key(device_id, name));" \

#define SQL_INSERT_SUPPORTED_DEVICES \
  "insert into supported_devices(" \
    "device_id, " \
    "device, " \
    "build_id, " \
    "check_property_name, " \
    "check_property_value) " \
  "values(" \
    "%d, " \
    "'%s', " \
    "'%s', " \
    "null, " \
    "null);"

#define SQL_INSERT_SUPPORTED_DEVICES_WITH_CHECK_PROPERTY \
  "insert into supported_devices(" \
    "device_id, " \
    "device, " \
    "build_id, " \
    "check_property_name, " \
    "check_property_value) " \
  "values(" \
    "%d, " \
    "'%s', " \
    "'%s', " \
    "'%s', " \
    "'%s');"

#define SQL_INSERT_DEVICES_ADDRESS \
  "insert into device_address(" \
    "device_id, " \
    "name, " \
    "value) " \
  "values(" \
    "%d, " \
    "'%s', " \
    "%u);"

#define WRITE_SQL_INSERT_DEVICE_ADDRESS(device,key) \
    if (device->key## _address) { \
      printf("  " SQL_INSERT_DEVICES_ADDRESS "\n", \
             device->device_id, \
	     #key, \
	     device->key## _address); \
    }

static int cmp(const void *a, const void *b)
{
  const supported_device *x = a; 
  const supported_device *y = b; 

  return x->device_id - y->device_id;
}

int main(void)
{
  int i;

  qsort(supported_devices, n_supported_devices, sizeof (supported_devices[0]), cmp);

  printf("%s\n\n", SQL_CREATE_SUPPORTED_DEVICES);
  printf("%s\n\n", SQL_CREATE_DEVICES_ADDRESS);

  for (i = 0; i < n_supported_devices; i++) {
    supported_device *device = &supported_devices[i];

    if (!device->check_property_name) {
      printf(SQL_INSERT_SUPPORTED_DEVICES "\n",
             device->device_id,
             device->device,
             device->build_id);
    }
    else {
      printf(SQL_INSERT_SUPPORTED_DEVICES_WITH_CHECK_PROPERTY "\n",
             device->device_id,
             device->device,
             device->build_id,
             device->check_property_name,
             device->check_property_value);
    }

    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, kernel_physical_offset)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, prepare_kernel_cred)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, commit_creds)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, remap_pfn_range)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, vmalloc_exec)
    WRITE_SQL_INSERT_DEVICE_ADDRESS(device, ptmx_fops)

    printf("\n");
  }

  return 0;
}
