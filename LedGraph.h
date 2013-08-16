#ifndef _LEDGRAPH_H_
#define _LEDGRAPH_H_

#include <avr/pgmspace.h>
#include "Constants.h"
#include "Pong3Table.h"

class LedGraph {
public:

  struct GraphNode {
    enum { MAX_NEIGHBORS=6 };

    Pong3Table::SegmentType segment_type;
    unsigned short neighbors[MAX_NEIGHBORS];
    unsigned char num_neighbors;
  };

  GraphNode GetNode(int node_num) const;

private:
  // Thanks to the weird data types, I think it's safer to store this as a
  // structure of arrays.
};

extern LedGraph ledGraph;

#endif // _LEDGRAPH_H_
