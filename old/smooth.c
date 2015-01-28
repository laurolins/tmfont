#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "smooth.h"

typedef struct ShortPair {
    short x;
    short y;
} ShortPair;

ShortPair ShortPair_init(short x, short y)
{
    ShortPair sp;
    sp.x = x;
    sp.y = y;
    return sp;
}

int  ShortPair_cmp(ShortPair a, ShortPair b)
{
    if (a.x < b.x)
	return -1;
    else if (a.x > b.x)
	return 1;
    else if (a.y < b.y)
	return -1;
    else if (a.y > b.y)
	return -1;
    else 
	return 0;
}

int  ShortPair_cmp_sqr_sum(ShortPair a, ShortPair b)
{
    int sqr_sum_a = a.x * a.x + a.y * a.y;
    int sqr_sum_b = b.x * b.x + b.y * b.y;
    return sqr_sum_a - sqr_sum_b;
}

void print_short_path_matrix(ShortPair *A, int m, int n)
{
    int i,j;
    for (i=0;i<m;i++)
    {
	for (j=0;j<n;j++)
	{
	    ShortPair sp = A[i*n + j];
	    printf("%2d,%2d  ",sp.x,sp.y);
	}
	printf("\n");
    }
}

void print_matrix(char *A, int m, int n)
{
    int i,j;
    for (i=0;i<m;i++)
    {
	for (j=0;j<n;j++)
	{
	    char x = A[i*n + j];
	    printf("%d ",(int) x);
	}
	printf("\n");
    }
}


void print_float_matrix(float *A, int m, int n)
{
    int i,j;
    for (i=0;i<m;i++)
    {
	for (j=0;j<n;j++)
	{
	    float x = A[i*n + j];
	    printf("%5.3f ",x);
	}
	printf("\n");
    }
}

/**
 * input:  a matrix of m lines and n columns with entries in {0, 1}
 * output: a matrix A of m lines and n columns where entry Aij is the minimum
 *         
 *
 * algorithm is O(m * n)          
 */
int is_undefined(ShortPair p)
{
    return p.x == -1;
}
int is_defined(ShortPair p)
{
    return p.x >= 0;
}

float *smooth(char *img, int m, int n, float R)
{
    ShortPair *A = (ShortPair*) malloc(m * n * sizeof(ShortPair));

    // horizontal pass
    ShortPair undefined_path = ShortPair_init(-1,-1);
    ShortPair best_path = undefined_path;

    int i,j,dim,direction;
    
    int limits[2] = {m, n};

    // initialize
    for (i=0;i<m;i++)
    {
	for (j=0;j<n;j++)
	{
	    int index = i*n + j;
	    if (img[index] == 1)
	    {
		A[index] = ShortPair_init(0,0);
	    }
	    else 
	    {
		A[index] = undefined_path;
	    }
	}
    }
    

    // ROUND 1
    for (dim=0;dim<2;dim++)
    {
    	int i_lim = limits[dim];
    	int j_lim = limits[(dim+1)%2];
    	for (i=0;i<i_lim;i++)
    	{
    	    for (direction=0;direction<2;direction++)
    	    {
    		int j0  = direction == 0 ?  0     : j_lim-1;
    		int j1  = direction == 0 ?  j_lim : -1;
    		int inc = direction == 0 ?  1     : -1;

    		int i_mult = dim == 0 ?  n : 1;
    		int j_mult = dim == 0 ?  1 : n;

    		best_path = undefined_path;
    		// printf("(j0=%d j1=%d inc=%d)\n",j0,j1,inc);
    		for (j=j0;j!=j1;j+=inc)
    		{
    		    /* printf("(%d %d) ",i,j); */
    		    int index = i*i_mult + j*j_mult;
    		    /* printf("(i=%d, j=%d) ",i,j); */
    		    if (img[index] == 1)
    		    {
    			best_path = ShortPair_init(0,0);
    		    }
    		    else if (is_defined(best_path))
    		    {
    			ShortPair Aindex = A[index];
    			if (is_undefined(Aindex) ||
    			    ShortPair_cmp_sqr_sum(Aindex, best_path) > 0)
    			{
    			    A[index] = best_path;
    			}
    		    }
		    
    		    // update best_path
    		    if (is_defined(best_path))
    		    {
    			if (dim == 0)
    			    best_path.x += 1;
    			else
    			    best_path.y += 1;
    		    }
    		}
    		/* printf("\n"); */
    	    }
    	}
    }
	
    // ROUND 2
    int kk;
    for (kk = 0;kk<2;kk++)
    {
    	for (dim=0;dim<2;dim++)
    	{
    	    int i_lim = limits[dim];
    	    int j_lim = limits[(dim+1)%2];
    	    for (i=0;i<i_lim;i++)
    	    {
    		for (direction=0;direction<2;direction++)
    		{
    		    int j0  = direction == 0 ?  0     : j_lim-1;
    		    int j1  = direction == 0 ?  j_lim : -1;
    		    int inc = direction == 0 ?  1     : -1;

    		    int i_mult = dim == 0 ?  n : 1;
    		    int j_mult = dim == 0 ?  1 : n;

    		    best_path = undefined_path;
    		    // printf("(j0=%d j1=%d inc=%d)\n",j0,j1,inc);
    		    for (j=j0;j!=j1;j+=inc)
    		    {
    			/* printf("(%d %d) ",i,j); */
    			int index = i*i_mult + j*j_mult;
    			/* printf("(i=%d, j=%d) ",i,j); */

    			if (is_defined(best_path))
    			{
    			    ShortPair Aindex = A[index];
    			    if (is_undefined(Aindex) ||
    				ShortPair_cmp_sqr_sum(Aindex, best_path) > 0)
    			    {
    				/* printf("replace %d,%d value %d,%d by %d,%d\n",i,j,Aindex.x,Aindex.y,best_path.x,best_path.y); */
    				A[index] = best_path;
    			    }
    			    else
    			    {
    				best_path = A[index];
    			    }
    			}
    			else
    			{
    			    best_path = A[index];
    			}
		    
    			// update best_path
    			if (is_defined(best_path))
    			{
    			    if (dim == 0)
    				best_path.x += 1;
    			    else
    				best_path.y += 1;
    			}
    		    }
    		    /* printf("\n"); */
    		}
    	    }
    	}
    }

    // print_short_path_matrix(A,m,n);

    // print matrix
    float *matrix = (float*) malloc(m * n * sizeof(float));
    /* float R = 5.0; */
    for (i=0;i<m;i++)
    {
	for (j=0;j<n;j++)
	{
	    int index = i*n + j;
	    ShortPair Aij = A[index];
	    float d = sqrt(Aij.x * Aij.x + Aij.y * Aij.y);
	    float norm_d = (R - d)/R;
	    if (d > R)
		matrix[index] = 0.0;
	    else
		matrix[index] = norm_d * norm_d;
	}
    }


    //
    free(A);
   
    //
    return matrix;
    
}
