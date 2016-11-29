#define REC_LEN	100
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cassert>
#include <time.h>
#include<sys/time.h>

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include "./libs/cJSON.h"
#include "./libs/projections.h"

//Pip header file 
#include "./libs/query_taxi.h"
//#include "./libs/pip.h"


//#include "VFTreeMin.h"
size_t mem_allocated=0;

using namespace std;
//static	sconfig s;
//static vftree *root=NULL;
//int *num_hier[DIM];

#define PORTNUM 9010
#define MAXHOSTNAME  100
#define MAXLINE  	 2048
#define MAXVERTICES 20

//double *point_x = NULL;
//double *point_y = NULL;
//int num_points = 0;

const string taxi_file = "taxi2013/";



/*
	Edge Class
	Takes  a source polygon index
	and a destination polygon index
	number of trips from src to dest
	number of trips starting from source only
	number of trips ending at destination only
 */

typedef struct {
	int src;
	int dst; 
	int id;
	int numTrips;
	int numSrcTrips;
	int numDstTrips;
} Edge;

void create_edge(Edge * e, int src , int dst, int id){
	e->src = src;
	e->dst = dst;
	e->id = id;
	e->numTrips = 0;
	e->numSrcTrips =0;
	e->numDstTrips=0;
}


// End of Edge Class

void fireman(int flag)
{
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

int establish(unsigned short portnum)
{
  char   myname[MAXHOSTNAME+1];
  int    s;
  struct sockaddr_in sa;
  struct hostent *hp;

  memset(&sa, 0, sizeof(struct sockaddr_in)); /* clear our address */
  gethostname(myname, MAXHOSTNAME);           /* who are we? */
  hp= gethostbyname(myname);                  /* get our address info */
  if (hp == NULL)                             /* we don't exist !? */
    return(-1);
  sa.sin_family= hp->h_addrtype;              /* this is our host address */
  sa.sin_port= htons(portnum);                /* this is our port number */
  if ((s= socket(AF_INET, SOCK_STREAM, 0)) < 0) /* create socket */
    return(-1);
    
  if (bind(s,(struct sockaddr *)&sa,sizeof(struct sockaddr_in)) < 0) {
    close(s);
    return(-1);                               /* bind address to socket */
  }
  //not work for the old linux kernel
  /*int option = 1;
  if(setsockopt(s,SOL_SOCKET,SO_REUSEPORT|SO_REUSEADDR,(char*)&option,sizeof(option)) < 0)
  {
	printf("setsockopt failed\n");
	close(s);
	exit(2);
  }*/  
  listen(s, 3);                               /* max # of queued connects */
  return(s);
}


int get_connection(int s)
{ int t;                  /* socket of connection */

  if ((t = accept(s,NULL,NULL)) < 0)   /* accept connection if there is one */
    return(-1);
  return(t);
}

void str_echo(int sockfd)
{
	printf("in str_echo sockfd=%d\n",sockfd);
	ssize_t n;
	char line[MAXLINE], temp[MAXLINE];
	{
		if ((n =read(sockfd, line, MAXLINE) ) == 0) return;
		line[n]='\0';

		char *p1=line,*p2=temp;
		for(int i=0;i<n;i++)
		{
			//printf("%c ",*p1);
			if(*p1!='\\') *p2++=*p1;
			p1++;
		}
		size_t len=p2-temp;
		temp[len]='\0';
		printf("temp=%s\n",temp);

	    cJSON* root = cJSON_Parse(temp);


	    /************ Processing Polygons *********/
		cJSON* features = cJSON_GetObjectItem(root,"vertices"); // this one is a collection of the things we need
		
		int npolygons = cJSON_GetArraySize(features);
		double p_array[npolygons][MAXVERTICES][2]; //Every vertex has an x and y coordinates thus the 2
		int nvertices_lut[npolygons]; 
		//This is a lookup table that tells us the number of vertices for a polygon.
		// We assume for polygon 0 nvertices_lut[0] will be its number of vertices

		for(int i=0; i < npolygons; i++)
		{
			cJSON* polygon = cJSON_GetObjectItem(cJSON_GetArrayItem(features,i),"geometry");
			cJSON* poly_coordinates = cJSON_GetObjectItem(polygon,"coordinates");
			char* typeofitem = cJSON_Print(poly_coordinates);
			//printf("\nPolygon number: %d, %s\n",i, typeofitem);
			cJSON* array = cJSON_GetArrayItem(poly_coordinates,0);

			int array_size = cJSON_GetArraySize(array);
			nvertices_lut[i] = array_size;

			for(int j = 0; j < array_size; j++)
			{
				cJSON* smallerarray = cJSON_GetArrayItem(array,j);
				double coord1 = (double)atof(cJSON_Print(cJSON_GetArrayItem(smallerarray,0)));
				double coord2 = (double)atof(cJSON_Print(cJSON_GetArrayItem(smallerarray,1)));
				char * testing = cJSON_Print(smallerarray);
				p_array[i][j][0] = coord1;
				p_array[i][j][1] = coord2;
				//printf("Vertex: %f , % f \n",p_array[i][j][0],p_array[i][j][1]);
			}
		}


		/********************** Processing Edges ****************************/
		cJSON * edges = cJSON_GetObjectItem(root,"edges"); //get the edges
		int nedges = cJSON_GetArraySize(edges);

		Edge edge_array[nedges];
		cJSON*  trips = cJSON_CreateArray();  //For the Json Response 

		for(int i=0; i < nedges; i ++)
		{
			cJSON* edge = cJSON_GetArrayItem(edges,i); //get the edge
			int src = atoi(cJSON_Print(cJSON_GetObjectItem(edge,"src")));
			int dst = atoi(cJSON_Print(cJSON_GetObjectItem(edge,"dst")));

			create_edge(&edge_array[i],src,dst,i);

			//Call the function to get the number of trips for the edges
			printf("Getting the trips for edges:::: \n");

			/*
			int pip_count2(
    			double polygon1[][2], //polygon 1
    			int num_vertices1,
    			double polygon2[][2], //polygon 2
    			int num_vertices2,
    			int &trips_count1, //counts for polygon 1
    			int &trips_count2, //counts for polygon 2
    			int &trips_count3 //counts for from 1 to 2
    			)

    		*/
			
			Edge* temp = &edge_array[i];
			pip_count2(p_array[src],nvertices_lut[src],p_array[dst],nvertices_lut[dst],temp->numSrcTrips
				, temp->numDstTrips, temp->numTrips);

			cJSON* temp_json = cJSON_CreateObject();
			cJSON_AddItemToObject(temp_json,"srctrips",cJSON_CreateNumber(temp->numSrcTrips));
			cJSON_AddItemToObject(temp_json,"dsttrips",cJSON_CreateNumber(temp->numDstTrips));
			cJSON_AddItemToObject(temp_json,"trips",cJSON_CreateNumber(temp->numTrips));
			cJSON_AddItemToArray(trips,temp_json);
			//cJSON_Delete(temp_json);

			printf("Trips are done!!! \n\n");

			printf("Edge: src = %d is dest = %d and numtrips: %d \n",temp->numSrcTrips,temp->numDstTrips, temp->numTrips);
		}


		//End of processing edges

		cJSON_Delete(root);
		//Now let us do the number of points processing using the function pip_count()
		//
		//
		//Now Let us do the transformation of coordinates
		
		//for(int i=0; i < npolygons; i++)
		//{
		//	for(int j=0; j < nvertices_lut[i]; j++) {
		//		double x = p_array[i][j][1];
		//		double y = p_array[i][j][0];
		//		GPS2NYC(x, y); //Latitude is y and Longitude is x
		//		p_array[i][j][0] = x;
		//		p_array[i][j][1] = y;
		//	}
		//}

		int countpoints[npolygons];
		for(int p =0; p < npolygons; p++){
			printf("Getting the number of points \n");
			timeval t0, t1;
			gettimeofday(&t0, NULL);
			countpoints[p] = pip_count(p_array[p], nvertices_lut[p]);
			gettimeofday(&t1, NULL);
			long total_cpu_time = t1.tv_sec * 1000000 + t1.tv_usec - t0.tv_sec * 1000000 - t0.tv_usec;
			printf("The number of points for polygon number: %d is %d\n",p,countpoints[p]);
			printf("pip_count completed in %10.2f ms......\n",(float)total_cpu_time/1000);
		}

		//Now we process the centroids
		double* centroids[npolygons]; //Array that will contain the centroids of all polygons
		for(int i = 0; i < npolygons; i++)
		{
			double* cent_temp = (double*) malloc (sizeof(double) * 2);
			//we take every polygon and loop through its vertices and get a centroid for that polygon
			//we do this with the help of the n_vertices_lut variable that contains the number of vertices of 
			//each polygon
			double centroid_x, centroid_y;
			for(int j =0; j < nvertices_lut[i]; j++)
			{
				centroid_x += p_array[i][j][0];
				centroid_y += p_array[i][j][1];
			}

			centroid_x /= (double)nvertices_lut[i];
			centroid_y /= (double)nvertices_lut[i];

			cent_temp[0] = centroid_x;
			cent_temp[1] = centroid_y;
			centroids[i] = cent_temp;
		}

		//for(int i=0; i < npolygons; i++)
		//{
		//	double x = centroids[i][1];
		//	double y = centroids[i][0];
		//	NYC2GPS(x, y);
		//	centroids[i][0] = x;
		//	centroids[i][1] = y;
		//}

		//for the sake of testing we will just use the first vertex of every polygon
		cJSON * json_response;
		json_response = cJSON_CreateObject();
		cJSON* coordarray = cJSON_CreateArray();
		cJSON* countarray = cJSON_CreateIntArray(countpoints,npolygons);

		for(int i =0; i < npolygons; i++)
		{
			cJSON* temp = cJSON_CreateDoubleArray(centroids[i],2);
			cJSON_AddItemToArray(coordarray,temp);
			free(centroids[i]);
		}

		cJSON_AddItemToObject(json_response,"coordinates",coordarray);
		cJSON_AddItemToObject(json_response,"numpoints",countarray);
		cJSON_AddItemToObject(json_response,"trips",trips);

		char * response = cJSON_Print(json_response);
		printf("\n\n The reponse string: %s\n",response);
		write(sockfd,response,strlen(response) + 1);

		cJSON_Delete(json_response);
		
		printf("Finished on str_echo pushed to client\n");
	}
}

int main()
{

  int s, t;
  printf("Main Program Running\n");
  if ((s= establish(PORTNUM)) < 0) {  /* plug in the phone */
    perror("establish");
    exit(1);
  }
  
  //loading points
  timeval t0, t1;
  printf("loading points...................\n");
  gettimeofday(&t0, NULL);
  //load_point(taxi_file, point_x, point_y, num_points);
  load_data(taxi_file.c_str());
  //TaxiData taxidata(taxi_file.c_str());
  gettimeofday(&t1, NULL);

  long point_loading_time = t1.tv_sec * 1000000 + t1.tv_usec - t0.tv_sec * 1000000 - t0.tv_usec;
  printf("point loading time=%10.2f",(float)point_loading_time/1000);
  fflush(stdout);

  signal(SIGCHLD, fireman);           /* this eliminates zombies */

  for (;;) {                          /* loop for phone calls */
    if ((t= get_connection(s)) < 0) { /* get a connection */
      if (errno == EINTR)             /* EINTR might happen on accept(), */
        continue;                     /* try again */
      perror("accept");               /* bad */
      exit(1);
    }
    switch(fork()) {                  /* try to handle connection */
    case -1 :                         /* bad news.  scream and die */
      perror("fork");
      close(s);
      close(t);
      exit(1);
    case 0 :                          /* we're the child, do something */
      close(s);
      str_echo(t);
      exit(0);
    default :                         /* we're the parent so look for */
      close(t);                       /* another connection */
      continue;
    }
  }

  return 0;
}


