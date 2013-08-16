NUM_LEDS = 421


def WriteSegmentType(graph, lines):
    replace_line = -1;
    for i, line in enumerate(lines):
        if line == "PROGMEM prog_uchar segment_type[NUM_LEDS] = {\n":
            replace_line = i + 1
    assert(replace_line != -1)

    new_line = []
    for node in graph:
        new_line.append("%d" % SEG_TYPES.index(node.seg_type))
    new_line = ','.join(new_line)
    new_line = "  %s\n" % new_line

    lines[replace_line] = new_line


def WriteNeighbors(graph, lines):
    replace_line = -1
    end_line = -1
    for i, line in enumerate(lines):
        if line == "PROGMEM prog_uint16_t neighbors[NUM_LEDS][LedGraph::GraphNode::MAX_NEIGHBORS] = {\n":
            replace_line = i + 1
        if replace_line != -1 and line == "};\n":
            end_line = i
            break
    assert(replace_line != -1)
    assert(end_line != -1)

    replacement_lines = []
    for node in graph:
        replacement_lines.append("  { %d, %d, %d, %d, %d, %d },\n" %
                                 tuple(node.neighbors))
    lines[replace_line:end_line] = replacement_lines


def WriteNumNeighbors(graph, lines):
    replace_line = -1;
    for i, line in enumerate(lines):
        if line == "PROGMEM prog_uchar num_neighbors[NUM_LEDS] = {\n":
            replace_line = i + 1
    assert(replace_line != -1)

    new_line = []
    for node in graph:
        new_line.append("%d" % node.num_neighbors)
    new_line = ','.join(new_line)
    new_line = "  %s\n" % new_line

    lines[replace_line] = new_line


def Main():
    lines = open("LedGraph.cpp", "rb").readlines()
    graph = BuildGraph()
    WriteSegmentType(graph, lines)
    WriteNeighbors(graph, lines)
    WriteNumNeighbors(graph, lines)
    open("LedGraph.cpp", "wb").writelines(lines)


class Segment:
    def __init__(self, start, end, seg_type, seg_num):
        self.start = start
        self.end = end
        self.seg_type = seg_type
        self.seg_num = seg_num

    @property
    def direction(self):
        if self.start < self.end:
            return 1
        return -1


ST_RADIAL = "Pong3Table::ST_RADIAL"
ST_BORDER = "Pong3Table::ST_BORDER"
SEG_TYPES = [ ST_RADIAL, ST_BORDER ]


def BuildSegments():
    segments = [
        Segment(34, 0, ST_RADIAL, 1),
        Segment(35, 69, ST_RADIAL, 0),
        Segment(70, 104, ST_BORDER, 0),
        Segment(105, 140, ST_BORDER, 1),
        Segment(141, 175, ST_BORDER, 2),
        Segment(210, 176, ST_RADIAL, 3),
        Segment(211, 245, ST_RADIAL, 2),
        Segment(246, 280, ST_BORDER, 3),
        Segment(315, 281, ST_RADIAL, 4),
        Segment(316, 350, ST_RADIAL, 5),
        Segment(385, 351, ST_BORDER, 4),
        Segment(386, 420, ST_BORDER, 5)
    ]
    return segments


def FindSegment(segments, led):
    for segment in segments:
        if ((segment.start < segment.end) and (led >= segment.start) and
            (led <= segment.end)):
            return segment
        if ((segment.start > segment.end) and (led <= segment.start) and
            (led >= segment.end)):
            return segment
    return None


def GetSegment(segments, seg_type, seg_num):
    for segment in segments:
        if seg_type == segment.seg_type and seg_num == segment.seg_num:
            return segment


class GraphNode:
    def __init__(self):
        self.seg_type = None
        self.neighbors = [0] * 6
        self.num_neighbors = 0


def BuildGraph():
    segments = BuildSegments()
    nodes = []
    for led in range(NUM_LEDS):
        node = GraphNode()
        segment = FindSegment(segments, led)
        node.seg_type = segment.seg_type

        if led == segment.start:
            if segment.seg_type == ST_RADIAL:
                for radial in range(6):
                    if radial == segment.seg_num:
                        continue
                    node.neighbors[node.num_neighbors] = GetSegment(segments, ST_RADIAL,
                        radial).start
                    node.num_neighbors += 1
                node.neighbors[5] = led + segment.direction
                node.num_neighbors = 6
            else:
                node.neighbors[0] = GetSegment(segments, ST_RADIAL, segment.seg_num).end
                node.neighbors[1] = GetSegment(segments, ST_BORDER,
                    (segment.seg_num + 5) % 6).end
                node.neighbors[2] = led + segment.direction
                node.num_neighbors = 3
        elif led == segment.end:
            if segment.seg_type == ST_RADIAL:
                node.neighbors[0] = GetSegment(segments, ST_BORDER, segment.seg_num).start
                node.neighbors[1] = GetSegment(segments, ST_BORDER,
                    (segment.seg_num + 5) % 6).end
                node.neighbors[2] = led - segment.direction
                node.num_neighbors = 3
            else:
                node.neighbors[0] = led - segment.direction
                node.neighbors[1] = GetSegment(segments, ST_BORDER,
                    (segment.seg_num + 1) % 6).start
                node.neighbors[2] = GetSegment(segments, ST_RADIAL,
                    (segment.seg_num + 1) % 6).end
                node.num_neighbors = 3
        else:
            node.neighbors[0] = led - segment.direction
            node.neighbors[1] = led + segment.direction
            node.num_neighbors = 2

        nodes.append(node)
    return nodes
                

if __name__ == "__main__":
    Main()

DONE = """
void Pong3Table::BuildGraph() {
  // One graph node per LED.
  for (int led = 0; led < NUM_LEDS; ++led) {
    // Find the segment for this LED.
    const LedSegment* found_segment = NULL;
    int segment_type;
    int segment_num;
    for (segment_type = ST_RADIAL;
	 segment_type < ST_NUM_TYPES;
	 ++segment_type) {
      for (segment_num = 0; segment_num < 6; ++segment_num) {
	const LedSegment* segment = GetSegment((SegmentType)segment_type,
					       segment_num);
	if (segment) {
	  if (segment->start <= segment->end) {
	    if (led >= segment->start && led <= segment->end) {
	      found_segment = segment;
	      break;
	    }
	  } else if(led >= segment->end && led <= segment->start) {
	      found_segment = segment;
	      break;
	  }
	}
      }
      if (found_segment) break;
    }

    // Make graph connections.
    int segment_dir = (found_segment->start <= found_segment->end) ? 1 : -1;
    if (led == found_segment->start) {
      switch(segment_type) {
        case ST_RADIAL:
	  for (int radial = 0, idx = 0; radial < 6; ++radial) {
	    if (radial == segment_num) continue;
	    graph_[led].neighbors[idx++] = GetSegment(ST_RADIAL, radial)->start;
	  }
	  graph_[led].neighbors[5] = led + segment_dir;
	  graph_[led].num_neighbors = 6;
	  break;

	  graph_[led].neighbors[0] = GetSegment(ST_RADIAL, segment_num)->end;
	  graph_[led].neighbors[1] = GetSegment(ST_BORDER,
						(segment_num + 5) % 6)->end;
	  graph_[led].neighbors[2] = led + segment_dir;
	  graph_[led].num_neighbors = 3;

          // TODO: Remove this hack:
          if (segment_num == 0) {
            graph_[led].neighbors[1] = graph_[led].neighbors[2];
            graph_[led].num_neighbors = 2;
          }
	  break;
      }
    } else if (led == found_segment->end) {
      switch(segment_type) {
        case ST_RADIAL:
	  graph_[led].neighbors[0] = GetSegment(ST_BORDER, segment_num)->start;
	  graph_[led].neighbors[1] = GetSegment(ST_BORDER,
						(segment_num + 5) % 6)->end;
	  graph_[led].neighbors[2] = led - segment_dir;
	  graph_[led].num_neighbors = 3;

          // TODO: Remove this hack:
          if (segment_num == 0) {
            graph_[led].neighbors[1] = graph_[led].neighbors[2];
            graph_[led].num_neighbors = 2;
          } else if (segment_num == 5) {
            graph_[led].neighbors[0] = graph_[led].neighbors[2];
            graph_[led].num_neighbors = 2;
          }
	  break;

        case ST_BORDER:
	  graph_[led].neighbors[0] = led - segment_dir;
	  graph_[led].neighbors[1] = GetSegment(ST_BORDER,
						(segment_num + 1) % 6)->start;
	  graph_[led].neighbors[2] = GetSegment(ST_RADIAL,
						(segment_num + 1) % 6)->end;
	  graph_[led].num_neighbors = 3;

          // TODO: Remove this hack:
          if (segment_num == 4) {
            graph_[led].neighbors[1] = graph_[led].neighbors[2];
            graph_[led].num_neighbors = 2;
          }
	  break;
      }
    } else {
      graph_[led].neighbors[0] = led - segment_dir;
      graph_[led].neighbors[1] = led + segment_dir;
      graph_[led].num_neighbors = 2;
    }
  }
}
"""
