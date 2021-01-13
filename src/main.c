#include <math.h>
#include <stdio.h>
#include <time.h>

#include "fortune.h"
#include "print_voronoi.h"
#include "animation.h"
#include "utils.h"

//#include "crtdbg.h" //for memory leak detection (Windows only)

double rand_interval(double a, double b)
{
	return (rand() / (double)RAND_MAX)*(b - a) + a;
}
void generate_random_points(char* filename, double left, double right, double bottom, double top, int n)
{
	FILE* fichier = NULL;
	fichier = fopen(filename, "w");
	if (fichier != NULL)
	{
		fprintf(fichier, "%d \n", n);
		for (int i = 0; i < n;i++)
		{
			double x = rand_interval(left, right);
			double y = rand_interval(bottom, top);
			fprintf(fichier, "%lf %lf \n", x, y);
		}
		fclose(fichier);
	}
	else
		printf("build file error");
}


int comp_point(const void * e1, const void * e2) {
	xy **p1 = (xy**) e1;
	xy **p2 = (xy**) e2;
	if((*p1)->y == (*p2)->y)
		return ((*p1)->x > (*p2)->x) - ((*p1)->x < (*p2)->x);
	else
		return ((*p1)->y > (*p2)->y) - ((*p1)->y < (*p2)->y);
}

///////////////////////////////////////////////////

xy** load_points(char* filename, int* n_sites)
{
	FILE* fichier = NULL;
	fichier = fopen(filename, "r");
	fscanf(fichier, "%d", n_sites);
	xy** points = (xy**)malloc((*n_sites) * sizeof(xy*));
	if (fichier != NULL)
	{
		for (int i = 0; i < *n_sites;i++)
		{
			double posR[2];
			fscanf(fichier, "%lf %lf", &posR[0], &posR[1]);
			points[i] = xy_new(posR[0], posR[1]);
		}
		fclose(fichier);

		qsort(points, *n_sites, sizeof(xy*), comp_point);
		for (int i = 0; i < *n_sites - 1; i++)
			if (points[i + 1]->y - points[i]->y < 1e-8) {
				points[i + 1]->y = points[i]->y + 1e-8;
			}

		return points;
	}
	else
		printf("file not found");
	return NULL;
}

int main(int argc, char *argv[]) {
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(42);

	//generate and load points
	generate_random_points("test.txt", 0, 100, 0, 100, 1000);//0, 10, 0, 10,50
	int n_sites = 0;
	xy** points = load_points("test.txt",&n_sites);

	//1. computation of Voronoi diagram without animation
	//DoublyConnectedEdgeList* dcel = fortuneAlgo(points, n_sites, NULL);
	//display_final_Voronoi(dcel,1,2);//(resize,flag)



	//2. computation of Voronoi diagram with animation
	Animation* animation = Animation_new(2, 0.01, 0.1,true); //Animation_new(2,0.001,0.08,true); //(resize,timeout,delta,stop_end)
	DoublyConnectedEdgeList* dcel = fortuneAlgo(points,n_sites,animation);
	Animation_delete(animation);


	DoublyConnectedEdgeList_print(dcel);
	DoublyConnectedEdgeList_delete(dcel);
	xy_tab_delete(points,n_sites);

	return EXIT_SUCCESS;
}
