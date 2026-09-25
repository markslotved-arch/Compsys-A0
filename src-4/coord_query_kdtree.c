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
    const struct kdtree_data *x = a;
    const struct kdtree_data *y = b;

    if (x->lon < y->lon)
        return -1;
    if (x->lon > y->lon)
        return 1;
    return 0;
}

int compLat(const void *a, const void *b) {
    const struct kdtree_data *x = a;
    const struct kdtree_data *y = b;

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

  node->left = build_kdtree(rs, median_index, depth + 1);

  node->right = build_kdtree(rs + median_index + 1, n - median_index - 1, depth + 1);

  return node;
}

/*

struct kdtree_data* mk_kdtree(struct record* rs, int n) { //source: https://ssojet.com/data-structures/implement-kd-tree-in-c#building-the-kd-tree
  struct kdtree_data* data = malloc(sizeof(struct kdtree_data));

    struct Node* build_kdtree(struct point points[], enum Axis lon, enum Axis lat) {
        if (n <= 0) return NULL;

        int axis = lon % 2;
        int median_index = n / 2;

        qsort(irs, n, sizeof(struct kdtree_data), compLon);

        
        // qsort(irs, n, sizeof(struct kdtree_data), compLat);
    }
    struct Node* node = create_node(points[median_index]);
        node->left = build_kdtree(points, median_index, depth + 1);
        node->right = build_kdtree(points + median_index + 1, n - median_index - 1, depth + 1);

    return node;
  };

// Procedure kdtree(points, depth)
// axis ← depth mod d();
// select median by axis from points;
// node ← new node;
// node.point ← median;
// node.axis ← axis;
// node.left ← kdtree (points before median, depth+1);
// node.right ← kdtree (points after median, depth+1);
// return node

// We let axis = 0 denote longtitude and axis = 1 denote latitude  

*/



void free_kdtree(struct kdtree_data* data) {
  free(data);
}




const struct record* lookup_kdtree(struct kdtree_data *data, double lon, double lat, int depth) {
struct needleLonLat needle;
  if (data -> root == NULL)
    return NULL;

  needle.lat = lat;
  needle.lon = lon;

  struct Node *root = data -> root;
  struct Node *nextbranch;
  struct Node *otherbranch;
  struct Node *temp;
  struct Node *best;
  struct Node nearest(struct Node *root, struct needleLonLat needle, int depth);
  double root_dist = distance(needle.lat, root->point.y, needle.lon, root->point.x);
  double temp_dist = distance(needle.lat, root->point.y, needle.lon, root->point.x);
  double difference;
 
  
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
 temp = nearest(nextbranch, needle, depth + 1);


if (temp_dist > root_dist){
  best = temp;
}
else{
  best = root;
}

double radius = distance(needle.lat, best->point.y, needle.lon, best->point.x);

// 4. calculate distance to splitting boundary
if (depth % 2 == 0){
  difference = needle.lon - root->point.x;
}
else {
  difference = needle.lat - root->point.y;
}

// 5. maybe search other side
if (radius >= (difference)) {
    temp = nearest(otherbranch, needle, depth + 1);

    if (temp_dist > root_dist){
  best = temp;
    }
    else{
      best = root;
    }
  }

// 6.
return best;
  

  // Procedure lookup(closest, query, node)
  // if node is NULL then
  // return
  // else if node.point is closer to query than closest then
  // replace closest with node.point;
  // diff ← node.point[node.axis] - query [node.axis];
  // radius ← the distance between query and closest
  // if diff ≥ 0 ∨ radius > |diff| then
  // lookup (closest, query, node.left)
  // if diff ≤ 0 ∨ radius > |diff| then
  // lookup (closest, query, node.right)

  // Here we assume that we can index a point with the axis (0 or 1) to obtain
  // the longtitude or latitude respectively. In code, this likely requires a branch.
  return NULL;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)build_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
