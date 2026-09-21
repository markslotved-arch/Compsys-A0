#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct binsort_record {
    int64_t osm_id;
    const struct record *record;
};

struct binsort_data {
    struct record *rs;
    struct binsort_record *irs;
    int n;
};

int comp(const void *a, const void *b) {
    const struct binsort_record *x = a;
    const struct binsort_record *y = b;

    if (x->osm_id < y->osm_id)
        return -1;
    if (x->osm_id > y->osm_id)
        return 1;
    return 0;
}

struct binsort_data* mk_binsort(struct record* rs, int n) {
  struct binsort_data* data = malloc(sizeof(struct binsort_data));
    struct binsort_record* irs = malloc(sizeof(struct binsort_record) * n);

  data -> rs = rs;
  data -> irs = irs;
  data -> n = n;

    for (int i = 0; i < n; i++) {
        irs[i].osm_id = rs[i].osm_id;
        irs[i].record = &rs[i];
    }

    qsort(irs, n, sizeof(struct binsort_record), comp);
    
    return data;
}

void free_binsort(struct binsort_data* data) {
  free(data -> irs);
  free(data);
}

const struct record* lookup_binsort(struct binsort_data *data, int64_t needle) {

    int low = 0;
    int high = data -> n - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2; 

            if (data->irs[mid].osm_id == needle){
                return data -> irs[mid].record;
            }
            else if (data->irs[mid].osm_id < needle){
                low = mid + 1;
            }
            else
                high = mid - 1;
        }
    return NULL;
}


int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_binsort,
                    (free_index_fn)free_binsort,
                    (lookup_fn)lookup_binsort);
}
