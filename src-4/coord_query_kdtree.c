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


struct needleLonLat {
  double lon;
  double lat;
};

struct point{
  double x;
  double y;
};

enum Axis {
  AXIS_LON,
  AXIS_LAT,
};

struct Node {
  struct point point;
  const struct record *rec;
  struct Node *left;
  struct Node *right;
  enum Axis axis;
};

struct kdtree_data {
  struct record *rs;
  double lon;
  double lat;
  struct Node *root;
  int n;
};

double distance(double x1, double x2, double y1, double y2) {
  return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)); 
}

int compLon(const void *a, const void *b) {
    const struct record *x = a;
    const struct record *y = b;

    if (x->lon < y->lon)
        return -1;
    if (x->lon > y->lon)
        return 1;
    return 0;
}

int compLat(const void *a, const void *b) {
    const struct record *x = a;
    const struct record *y = b;

    if (x->lat < y->lat)
        return -1;
    if (x->lat > y->lat)
        return 1;
    return 0;
}

struct Node* build_kdtree(struct record* rs, int n, int depth) {
  // rs er selveste records
  // n er hvor mange records vi har
  // depth er hvor dybt vi er i træet

  // base case
  if (n == 0) {
    return NULL;
  }

  // bestemmer om det skal være lon eller lat
  int axis = depth % 2;

  // sortere med enten lon eller lat
  if (axis == 0) {
    qsort(rs, n, sizeof(struct record), compLon);
  }
  else qsort(rs, n, sizeof(struct record), compLat);

  // finder median efter den er sorteret
  int median_index = n / 2;

  // giver plads til selveste noden
  struct Node* node = malloc(sizeof(struct Node));

  if (axis == 0) {
    node->axis = AXIS_LON;
  } else { 
    node->axis = AXIS_LAT;
  }

  // sætter point x og y
  node->point.x = rs[median_index].lon;
  node->point.y = rs[median_index].lat;

  // gemmer en pointer til recorden
  node->rec = &rs[median_index];

  node->left = build_kdtree(rs, median_index, depth + 1);

  node->right = build_kdtree(rs + median_index + 1, n - median_index - 1, depth + 1);

  return node;
}

struct kdtree_data* mk_kdtree(struct record* rs, int n) {
  struct kdtree_data *data = malloc(sizeof(struct kdtree_data));
  data->rs = rs;
  data->n = n;
  data->lon = 0;
  data->lat = 0;
  data->root = build_kdtree(rs, n, 0);
  return data;
}

void free_nodes(struct Node *node) {
  if (node == NULL)
    return;
  free_nodes(node->left);
  free_nodes(node->right);
  free(node);
}

void free_kdtree(struct kdtree_data* data) {
  free_nodes(data->root);
  free(data);
}

const struct record* lookup_node(struct Node *root, double lon, double lat, int depth) {
  struct needleLonLat needle;
  //base case
  if (root == NULL)
    return NULL;

  needle.lat = lat;
  needle.lon = lon;

  //nextbranch er den den af træet vi søger i
  struct Node *nextbranch;
  //otherbranch er det modsatte sjovt nok
  struct Node *otherbranch;
  struct Node *temp;
  struct Node *best;

  //disse er bare beregningen af distancen på root og på temp
  double root_dist = distance(needle.lat, root->point.y, needle.lon, root->point.x);
  double temp_dist;
  double difference;
 
  // her vælger vi så om vi kigger lon eller lat, lon hvis depth kan deles med 2 og lat hvis den ikke kan
  if (depth % 2 == 0) {
   if(needle.lon < root->point.x){
      nextbranch = root->left;
      otherbranch = root->right;}
    
    else{
      nextbranch = root->right;
      otherbranch = root->left;
    }

  }
  else{
    if(needle.lat < root->point.y){
      nextbranch = root->left;
      otherbranch = root->right;}

    else{
      nextbranch = root->right;
      otherbranch = root->left;
    }
  }

  // søg i næste gren
  temp = lookup_node(nextbranch, lon, lat, depth + 1);
  //her ændre vi vores "root" til at være next branch så vi kan søge vidre
  best = root;
  if (temp != NULL) {
    temp_dist = distance(needle.lat, temp->point.y, needle.lon, temp->point.x);
    if (temp_dist < root_dist) {
      best = temp;
    }
  }

  double radius = distance(needle.lat, best->point.y, needle.lon, best->point.x);

  //det her er så det der kdtree noget hvor man ser på linjerne ik og så depending on om vi ser på lon eller lat ved denne depth, vælger vi så lon eller lat
  if (depth % 2 == 0){
    difference = needle.lon - root->point.x;
  }
  else {
    difference = needle.lat - root->point.y;
  }

  //her ser vi om vores "best" er god nok, eller om vi skal tjekke den anden del af kdtræet ud!
  if (radius >= fabs(difference)) {
    temp = lookup_node(otherbranch, lon, lat, depth + 1);

    if (temp != NULL) {
      temp_dist = distance(needle.lat, temp->point.y, needle.lon, temp->point.x);
      if (temp_dist < radius){
        best = temp;
      }
    }
  }
  return best;  
}

const struct record* lookup_kdtree(struct kdtree_data *data, double lon, double lat) {
  struct Node *best = lookup_node(data->root, lon, lat, 0);
 
  if (best == NULL)
    return NULL;
 
  return best->rec;
}


int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)build_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
