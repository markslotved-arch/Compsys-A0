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


struct naive_data {
  struct record *rs;
  int n;
  double lon;
  double lat;
};

struct needleLonLat {
  double lon;
  double lat;
};
struct comparingLonLat {
  double lon;
  double lat;
};

struct naive_data* mk_naive(struct record* rs, int n) {
  struct naive_data* data = malloc(sizeof(struct naive_data));

  data -> rs = rs;
  data -> n = n;

  return data;
}

void free_naive(struct naive_data* data) {
  free(data);
}


double distance(double x1, double x2, double y1, double y2) {
  return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); 
}

const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
  struct needleLonLat needle;
  struct comparingLonLat comparing;
  struct record *closest_record = NULL;

  double closestDistance = INFINITY; // source: https://stackoverflow.com/questions/1923837/how-to-use-nan-and-inf-in-c
  
  needle.lat = lat;
  needle.lon = lon;

  for (int i = 0; i < data->n; i++) {
    comparing.lat = data -> rs[i].lat;
    comparing.lon = data -> rs[i].lon;
    double dist = distance(needle.lat, comparing.lat, needle.lon, comparing.lon);    
    if (dist < closestDistance) {
      closestDistance = dist; 
      closest_record = &data->rs[i];
    }
  }
    return closest_record;          
    return NULL;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
