#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

struct kdtree_data {
  struct record *rs;
  int n;
};

struct point{
  int x;
  int y;
};

struct kdtree_data* mk_kdtree(struct record* rs, int n) {
  struct kdtree_data* data = malloc(sizeof(struct kdtree_data));

  data -> rs = rs;
  data -> n = n;

  return data;
}

void free_kdtree(struct kdtree_data* data) {
  free(data);
}


double distance(double x1, double x2, double y1, double y2) {
  return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); 
}

const struct record* lookup_kdtree(struct kdtree_data *data, double lon, double lat) {
       
    return NULL;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
struct point{
  int x;
  int y;
  int z;
};

int read_point(struct point *p, FILE *f) {
  char *line = NULL;
  size_t n;
  if (getline(&line, &n, f) == -1) {
    free(line);
    return -1;
  }

  char* start = line;
  char* end;

  if ((end = strstr(start, "\t"))) {
    p->x = atoi(start); 
    *end = 0; 
    start = end+1;
  }

  if ((end = strstr(start, "\t"))) {
    p->y = atoi(start); 
    *end = 0; 
    start = end+1;
  }

  if ((end = strstr(start, "\n"))) {
    p->z = atoi(start); 
    *end = 0; 
    start = end+1;
  }

  return 0;
}

struct point* read_points(const char *filename, int *n) {
  FILE *f = fopen(filename, "r");
  *n = 0;

  if (f == NULL) {
    return NULL;
  }

  int capacity = 100;
  int i = 0;
  struct point *rs = malloc(capacity * sizeof(struct point));
  while (read_point(&rs[i], f) == 0) {
    i++;
    if (i == capacity) {
      capacity *= 2;
      rs = realloc(rs, capacity * sizeof(struct point));
    }
  }

  *n = i;
  fclose(f);
  return rs;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <DATA>\n", argv[0]);
    exit(1);
  }
  int n;

  struct point* points = read_points(argv[1], &n);

  printf("Read %d points\n", n);

  free(points);
}