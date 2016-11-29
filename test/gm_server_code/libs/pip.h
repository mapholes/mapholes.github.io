#ifndef PIP_H_
#define PIP_H_
#include <limits>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace std;

void load_point(const string &path, double* &x, double* &y, int &size) {
  //meta file
  string meta_file_path = path + "meta";
  FILE *fp = fopen(meta_file_path.c_str(), "r");
  if (fp == NULL) {
    fprintf(stderr, "%s not found\n", meta_file_path.c_str());
  }
  fseek(fp, 0, SEEK_END); // seek to end of file
  size_t file_size = ftell(fp); // get current file pointer
  fseek(fp, 0, SEEK_SET); // seek back to beginning of file

  char *buffer = new char[file_size];
  size_t read_sz = fread(buffer, sizeof(char), file_size, fp);
  fclose(fp);
  int num_points = atoi(buffer);
  delete []buffer;

  size = num_points;
  //cout<<"number of points: "<<num_points<<endl;
  //id = new int[num_points];
  x = new double[num_points];
  y = new double[num_points];
  //load data from file...
  //id
  //string id_file_path = path + "id";
  //fp = fopen(id_file_path.c_str(), "rb");
  //read_sz = fread(id, sizeof(int), num_points, fp);
  //fclose(fp);

  //x
  string x_file_path = path + "x";
  fp = fopen(x_file_path.c_str(), "rb");
  if (fp == NULL) {
    fprintf(stderr, "%s not found\n", x_file_path.c_str());
  }
  read_sz = fread(x, sizeof(double), num_points, fp);
  fclose(fp);

  //y
  string y_file_path = path + "y";
  fp = fopen(y_file_path.c_str(), "rb");
  if (fp == NULL) {
    fprintf(stderr, "%s not found\n", y_file_path.c_str());
  }
  read_sz = fread(y, sizeof(double), num_points, fp);
  fclose(fp);


  //print out...
  for (int i = 0; i < num_points; i++) {
    if(i % 10000000 == 0){
    printf("%lf %lf\n", x[i], y[i]);
    }
  }

}

int pip_count(double vertices[][2], 
		int num_vertices, 
		double *point_x, 
		double *point_y, 
		int num_points) {
  //get MBR first
  double xmin = std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double xmax = -std::numeric_limits<double>::max();
  double ymax = -std::numeric_limits<double>::max();
  for (int i = 0; i < num_vertices; ++i) {
    xmin = std::min(xmin, vertices[i][0]);
    ymin = std::min(ymin, vertices[i][1]);
    xmax = std::max(xmax, vertices[i][0]);
    ymax = std::max(ymax, vertices[i][1]);

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

  fprintf(stderr, "num of points: %d\n", num_points);

  int count = 0;
#pragma omp parallel for reduction(+:count)
  for (int i = 0; i < num_points; ++i) {
    double x = point_x[i];
    double y = point_y[i];
    if (x < xmin || x > xmax || y < ymin || y > ymax) continue;
    bool in_polygon = false;
    for (int j = 0; j < num_vertices-1; ++j) {
      double x0 = vertices[j][0];
      double x1 = vertices[j+1][0];
      double y0 = vertices[j][1];
      double y1 = vertices[j+1][1];

      if ((((y0 <= y) && (y < y1)) ||
            ((y1 <= y) && (y < y0))) &&
          (x < (x1 - x0) * (y - y0) / (y1 - y0) + x0))
        in_polygon = !in_polygon;
    }

    if (in_polygon) ++count;
  }
  return count;

}

#endif

