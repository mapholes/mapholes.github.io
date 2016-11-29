#ifndef PROJECT_H
#define PROJECT_H
#include <cassert>
#include <ogr_spatialref.h>

void transform_projection(double input_x, double input_y, 
                          double &output_x, double &output_y,
                          int source_projection, int target_projection) {
  OGRSpatialReference oSourceSRS, oTargetSRS;
  OGRCoordinateTransformation *poCT;
  double                  x, y;

  oSourceSRS.importFromEPSG( source_projection );
  oTargetSRS.importFromEPSG( target_projection );

  poCT = OGRCreateCoordinateTransformation( &oSourceSRS,
      &oTargetSRS );
  x = input_x;
  y = input_y;

  if( poCT == NULL || !poCT->Transform( 1, &x, &y ) ) {
    fprintf(stderr, "Transformation failed.\n" );
    //return 1;
  }
  output_x = x;
  output_y = y;
  //return 0;
}

void GPS2NYC(double &x, double &y) {
  printf("%lf %lf\n",x,y);
  //assert(x>-180&&x<0&&y>0&&y<90);
  double input_x = x;
  double input_y = y;
  transform_projection(input_x, input_y, x, y, 4326, 2263);
  printf("transformed %lf %lf\n",x,y);
}
void NYC2GPS(double &x, double &y) {
  double input_x = x;
  double input_y = y;
  transform_projection(input_x, input_y, x, y, 2263, 4326);
}

#endif
