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
  double lon;
  double lat;
  struct Node *root;
  int n;
};

enum Axis {
  lon,
  lat,
};

struct point{
  int x;
  int y;
};

struct Node {
  struct point;
  struct Node *left;
  struct Node *right;
  enum Axis;
};

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

struct kdtree_data* mk_kdtree(struct record* rs, int n) { //source: https://ssojet.com/data-structures/implement-kd-tree-in-c#building-the-kd-tree
  struct kdtree_data* data = malloc(sizeof(struct kdtree_data));

    struct Node* build_kdtree(struct point points[], enum Axis lon, enum Axis lat) {
        if (n <= 0) return NULL;

        int axis = lon % 2;
        int median_index = n / 2;

        qsort(irs, n, sizeof(struct kdtree_data), compLon);

        
        // qsort(irs, n, sizeof(struct kdtree_data), compLat);

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

  


}

void free_kdtree(struct kdtree_data* data) {
  free(data);
}




const struct record* lookup_kdtree(struct kdtree_data *data, double lon, double lat) {
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
                          (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree,
                          (lookup_fn)lookup_kdtree);
}
