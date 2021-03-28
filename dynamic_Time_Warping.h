#pragma once
#include "os/lib/heapmem.h"
#include "recorder.h"
#include <math.h>
#include "sys/log.h"


#define LOG_MODULE "DWT Log"
#define LOG_LEVEL LOG_LEVEL_DBG
/*
    calcualtes the pairwise Euclidean distance between two sequences.
*/
char** compute_euclidean_distance_matrix(char stored_password[], char stored_login[]);
char** compute_accumulated_cost_matrix(char stored_password[], char stored_login[]);
void print_matrix(char **);
int min(int cost1, int cost2, int cost3);

char** compute_euclidean_distance_matrix(char stored_password[], char stored_login[])
{
    // initializing 2-dim array on heap
    LOG_DBG("init 2-dim on heap \n");
    char **distance_matrix = (char **)malloc(SIZE * sizeof(char*));
    int init_counter = 0;
    while(init_counter < SIZE)
    {
        distance_matrix[init_counter] = (char *)malloc(SIZE * sizeof(char));
    }

    int r = SIZE;
    int c = SIZE;
    int i = 0;
    int j = 0;
    while (i<r)
    {
        while (j < c)
        {
            distance_matrix[i][j] = pow((stored_password[j] - stored_login[i]), 2);
            j++;
        }
        i++;  
    }
    LOG_DBG("Calculation of distance matrix was a succes \n");
    return distance_matrix;
}


char** compute_accumulated_cost_matrix(char stored_password[], char stored_login[])
{
    // calc distance matrix
    char** dist_matrix = compute_euclidean_distance_matrix(stored_password, stored_login);

    // allocate cos matrix
    char **cost_matrix = (char **)malloc(SIZE * sizeof(char*));
    int init_counter = 0;
    while(init_counter < SIZE)
    {
        cost_matrix[init_counter] = (char *)malloc(SIZE * sizeof(char));
    }

    // Init cost matrix
    cost_matrix[0][0] = dist_matrix[0][0];

    int i = 0;
    while(i<SIZE)
    {
        cost_matrix[i][0] = dist_matrix[i][0] + cost_matrix[i-1][0];  
        i++;
    }

    int j = 0;
    while(j<SIZE)
    {
        cost_matrix[0][j] = dist_matrix[0][j] + cost_matrix[0][j-1];  
        j++;
    }

    i = 0;
    j = 0;

    while (i<SIZE)
    {
        while (j < SIZE)
        {
            cost_matrix[i][j] = min(cost_matrix[i-1][j],    
                                    cost_matrix[i][j-1],   
                                    cost_matrix[i-1][j-1]) +  dist_matrix[i][j];
            j++;
        }
        i++;  
    }

    return cost_matrix;
}


void print_matrix(char** matrix)
{
    int r = SIZE;
    int c = SIZE;
    int i = 0;
    int j = 0;
    while (i<r)
    {
        while (j < c)
        {
            printf("%d \n", matrix[i][j]);
            j++;
        }  
        i++;
    }
}


int min(int cost1, int cost2, int cost3){
    if (cost1 <= cost2 && cost1 <= cost3)
    {
        return cost1;
    }
    else if(cost2 <= cost1 && cost2 <= cost3)
    {
        return cost2;
    }
    else
    {
        return cost3;
    }
}



