#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef enum KDM_DATA_TYPE_E
  {
    KDM_DATA_TYPE_NOMINAL=1,
    KDM_DATA_TYPE_NUMERIC=2,
    KDM_DATA_TYPE_UNKNOWN=0
  } KDM_DATA_TYPE_T;

typedef struct kdm_data_cell_s
{
  KDM_DATA_TYPE_T type;
  char *nominal;
  double numeric;
} kdm_data_cell_t;

kdm_data_cell_t*
kdm_data_cell_init0 (void);

kdm_data_cell_t*
kdm_data_cell_init_nominal (const char *nominal);

kdm_data_cell_t*
kdm_data_cell_init_numeric (double numeric);

void
kdm_data_cell_free (kdm_data_cell_t* ptr);

typedef struct kdm_raw_data_table_s
{
  size_t attribute_count;
  char **attribute_names;
  size_t row_count;
  kdm_data_cell_t **rows;
} kdm_raw_data_table_t;

kdm_raw_data_table_t*
kdm_raw_data_table_init (size_t attribute_count, char **attribute_names,
			 size_t row_count, kdm_data_cell_t **rows);

void
kdm_raw_data_table_free (kdm_raw_data_table_t);

int
main (int argc, char **argv)
{
  return 0;
}

kdm_data_cell_t*
kdm_data_cell_init0 (void)
{
  kdm_data_cell_t* result = malloc (sizeof(kdm_data_cell_t));
  assert(result != NULL);
  memset (result, 0, sizeof (kdm_data_cell_t));
  return result;
}

kdm_data_cell_t*
kdm_data_cell_init_nominal (const char *nominal)
{
  kdm_data_cell_t* result = kdm_data_cell_init0 ();
  result->type = KDM_DATA_TYPE_NOMINAL;
  result->nominal = strdup (nominal);
  return result;
}

kdm_data_cell_t*
kdm_data_cell_init_numeric (double numeric)
{
  kdm_data_cell_t* result = kdm_data_cell_init0 ();
  result->type = KDM_DATA_TYPE_NUMERIC;
  result->numeric = numeric;
  return result;
}

void
kdm_data_cell_free (kdm_data_cell_t* ptr)
{
  if (ptr)
    free (ptr->nominal);
  free (ptr);
}
