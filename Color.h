#ifndef _COLOR_H_
#define _COLOR_H_

struct Color {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;

  Color() : red(0), green(0), blue(0), alpha(0) {}
  Color(int r, int g, int b) : red(r), green(g), blue(b), alpha(255) {}
  Color(int r, int g, int b, int a) : red(r), green(g), blue(b), alpha(a) {}
  Color(const Color& c) : red(c.red), green(c.green), blue(c.blue),
    alpha(c.alpha) {}
};

#endif // _COLOR_H_
