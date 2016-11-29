#ifndef QUERY_TAXI_H
#define QUERY_TAXI_H
#include <iostream>
#include <cassert>
#include <string>
#include <cstdio>
#include <fstream>
#include <limits>

typedef unsigned int uint;

typedef struct Time
{
    uint y : 6;
    uint m : 4;
    uint d : 5;
    uint hh : 5;
    uint mm : 6;
    uint ss : 6;
}Time;

//class TaxiData {
//public:
Time *pickup_time, *dropoff_time;
double *pickup_x, *pickup_y, *dropoff_x, *dropoff_y;
int *ids;
size_t total_size;

//TaxiData(const char *input) {
//  load_data(input);
//}
//~TaxiData() {
//  delete[] pickup_time;
//  delete[] dropoff_time;
//  delete[] pickup_x;
//  delete[] pickup_y;
//  delete[] dropoff_x;
//  delete[] dropoff_y;
//  delete[] ids;
//}

using namespace std;

void load_data(const char *input) {
  string input_folder(input);
  string id_file = input_folder + "/id";
  FILE *fp = fopen(id_file.c_str(), "rb");
  fseek(fp, 0L, SEEK_END);
  total_size = ftell(fp) / sizeof(int);
  fseek(fp, 0L, SEEK_SET);
  pickup_time = new Time[total_size];
  dropoff_time = new Time[total_size];
  pickup_x = new double[total_size];
  pickup_y = new double[total_size];
  dropoff_x = new double[total_size];
  dropoff_y = new double[total_size];
  ids = new int[total_size];
  fread(ids, sizeof(int), total_size, fp);
  fclose(fp);
  
  string file = input_folder + "/pt";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(pickup_time, sizeof(Time), total_size, fp);
  fclose(fp);
  printf("done pt\n");

  file = input_folder + "/dt";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(dropoff_time, sizeof(Time), total_size, fp);
  fclose(fp);
  printf("done dt\n");

  file = input_folder + "/px";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(pickup_x, sizeof(double), total_size, fp);
  fclose(fp);

  printf("done px\n");
  file = input_folder + "/py";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(pickup_y, sizeof(double), total_size, fp);
  fclose(fp);

  printf("done py\n");
  file = input_folder + "/dx";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(dropoff_x, sizeof(double), total_size, fp);
  fclose(fp);

  printf("done dx\n");
  file = input_folder + "/dy";
  fp = fopen(file.c_str(), "rb");
  assert(fp != NULL);
  fread(dropoff_y, sizeof(double), total_size, fp);
  fclose(fp);
  printf("done dy\n");
}

bool pip(double x, double y, double vertices[][2], int num_vertices,
         double xmin, double ymin, double xmax, double ymax) {
  if (x < xmin || x > xmax || y < ymin || y > ymax) return false;
  bool in_polygon = false;
  for (int j = 0; j < num_vertices-1; ++j) {
    double x0 = vertices[j][1];
    double x1 = vertices[j+1][1];
    double y0 = vertices[j][0];
    double y1 = vertices[j+1][0];

    if ((((y0 <= y) && (y < y1)) ||
          ((y1 <= y) && (y < y0))) &&
        (x < (x1 - x0) * (y - y0) / (y1 - y0) + x0))
      in_polygon = !in_polygon;
  }
  return in_polygon;

}

int pip_count(double vertices[][2], 
		int num_vertices) {
  //get MBR first
  double xmin = std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double xmax = -std::numeric_limits<double>::max();
  double ymax = -std::numeric_limits<double>::max();
  for (int i = 0; i < num_vertices; ++i) {
    xmin = std::min(xmin, vertices[i][1]);
    ymin = std::min(ymin, vertices[i][0]);
    xmax = std::max(xmax, vertices[i][1]);
    ymax = std::max(ymax, vertices[i][0]);

  }


  //load points
//  if (point_x == NULL && point_y == NULL) {
//	printf("loading points...................\n");
//	timeval t0, t1;
//	gettimeofday(&t0, NULL);
//    load_point(taxi_file, point_x, point_y, num_points);
//    long point_loading_time = t1.tv_sec * 1000000 + t1.tv_usec - t0.tv_sec * 1000000 - t0.tv_usec;
//    printf("point loading time=%10.2f",(float)point_loading_time/1000);
//  }

  fprintf(stderr, "num of points: %d\n", total_size);

  int count = 0;
#pragma omp parallel for reduction(+:count)
  for (int i = 0; i < total_size; ++i) {
    double x = pickup_x[i];
    double y = pickup_y[i];
    if (x < xmin || x > xmax || y < ymin || y > ymax) continue;
    bool in_polygon = false;
    for (int j = 0; j < num_vertices-1; ++j) {
      double x0 = vertices[j][1];
      double x1 = vertices[j+1][1];
      double y0 = vertices[j][0];
      double y1 = vertices[j+1][0];

      if ((((y0 <= y) && (y < y1)) ||
            ((y1 <= y) && (y < y0))) &&
          (x < (x1 - x0) * (y - y0) / (y1 - y0) + x0))
        in_polygon = !in_polygon;
    }

    if (in_polygon) ++count;
  }
  return count;

}


int pip_count2(
    double polygon1[][2], //polygon 1
    int num_vertices1,
    double polygon2[][2], //polygon 2
    int num_vertices2,
    int &trips_count1, //counts for polygon 1
    int &trips_count2, //counts for polygon 2
    int &trips_count3 //counts for from 1 to 2
    ) {
  //get MBRs
  double xmin1 = std::numeric_limits<double>::max();
  double ymin1 = std::numeric_limits<double>::max();
  double xmax1 = -std::numeric_limits<double>::max();
  double ymax1 = -std::numeric_limits<double>::max();
  for (int i = 0; i < num_vertices1; ++i) {
    xmin1 = std::min(xmin1, polygon1[i][1]);
    ymin1 = std::min(ymin1, polygon1[i][0]);
    xmax1 = std::max(xmax1, polygon1[i][1]);
    ymax1 = std::max(ymax1, polygon1[i][0]);
  }
  //cout <<"query polygon 1: ";
  //cout <<xmin1 << " " << ymin1 << " " <<xmax1 << " " << ymax1 << endl;
  //cout << "test: ";
  //cout <<pickup_x[0] << " " << pickup_y[0] << endl;
  //cout <<dropoff_x[0] << " " << dropoff_y[0] << endl;

  double xmin2 = std::numeric_limits<double>::max();
  double ymin2 = std::numeric_limits<double>::max();
  double xmax2 = -std::numeric_limits<double>::max();
  double ymax2 = -std::numeric_limits<double>::max();
  for (int i = 0; i < num_vertices2; ++i) {
    xmin2 = std::min(xmin2, polygon2[i][1]);
    ymin2 = std::min(ymin2, polygon2[i][0]);
    xmax2 = std::max(xmax2, polygon2[i][1]);
    ymax2 = std::max(ymax2, polygon2[i][0]);
  }

  int count1 = 0;
  int count2 = 0;
  int count3 = 0;
 
#pragma omp parallel for reduction(+:count1, count2, count3)
  for (int i = 0; i < (int)total_size; ++i) {
    bool in_polygon_1 = pip(pickup_x[i], pickup_y[i], polygon1, num_vertices1,
                            xmin1, ymin1, xmax1, ymax1);
    bool in_polygon_2 = pip(dropoff_x[i], dropoff_y[i], polygon2, num_vertices2,
                            xmin2, ymin2, xmax2, ymax2);
    if (in_polygon_1) count1++;
    if (in_polygon_2) count2++;
    if (in_polygon_1 && in_polygon_2) count3++;

  }
  trips_count1 = count1;
  trips_count2 = count2;
  trips_count3 = count3;
  return 0;
}


//};

#endif
