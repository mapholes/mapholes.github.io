
#include <cstdio>

#include "pip.h"

int main() {
  double vertices[][2] = {
{988446.355050,216829.024257},
{991728.547460,214082.016913},
{994295.671084,216925.467289},
{990063.206219,217397.717033},
{988446.355050,216829.024257} 
}   ;
  double vertices2[][2] = {
{988446.355050,216829.024257},
{991728.547460,214082.016913},
{994295.671084,216925.467289},
{990063.206219,217397.717033},
{988446.355050,216829.024257} 
}   ;

  fprintf(stdout, "start\n");
  int count = pip_count(vertices, 5);
  fprintf(stderr, "count: %d\n", count);
  count = pip_count(vertices2, 5);
  fprintf(stderr, "count: %d\n", count);

  return 0;
}
