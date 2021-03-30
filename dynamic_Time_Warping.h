#pragma once
#include "os/lib/heapmem.h"
#include "recorder.h"
#include <math.h>
#include "sys/log.h"

#define LOG_MODULE "Knock "
#define LOG_LEVEL LOG_LEVEL_DBG


/*
    Calcualtes the pairwise Euclidean distance between two sequences.

    $$Params$$
        stored_password : the stored secret password
        stored_login : the stored current trail login

    $$return$$
        returns the pairwise Euclidean distance between two passwords
*/
int **compute_euclidean_distance_matrix(char stored_password[], char stored_login[]);


/*
    Compute accumulated cost matrix for warp path using Euclidean distance

    $$Params$$
        stored_password : the stored secret password
        stored_login : the stored current trail login

    $$return$$
        returns the pairwise Euclidean distance between two passwords
*/
int **compute_accumulated_cost_matrix(char stored_password[], char stored_login[]);


/*
    Pretty prints matrix

    $$return$$
        void        
*/
void print_matrix(int **);


/*
    Calculates the minumin of 3 intergers

    $$Params$$
        cost1, cost2, cost3 : numbers to compare

    $$return$$
        returns the minimum of the 3 numbers
*/
int min(int cost1, int cost2, int cost3);


/*
    Accumulates the lowest cost path, on the cost matrix

    $$Params$$
        cost_matrix : cost matrix

    $$return$$
        returns the accumulated minimum cost
*/
int min_cost_warping_path(int **cost_matrix);


/*
    Free the dynamically allocated memory

    $$Params$$
        cost_matrix : cost matrix

    $$return$$
        returns the accumulated minimum cost
*/
void free_matrix(int** matrix);


/*------------IMPLEMENTATION START--------------*/
/*------------IMPLEMENTATION START--------------*/
/*------------IMPLEMENTATION START--------------*/
int **compute_euclidean_distance_matrix(char stored_password[], char stored_login[])
{
    // initializing 2-dim array on heap
    LOG_DBG("init dist matrix on heap \n");
    int rows = SIZE;
    int cols = SIZE;
    int **distance_matrix = (int **)malloc(rows * sizeof(int *));
    int counter = 0;
    while (counter < rows)
    {
        distance_matrix[counter] = (int *)malloc(cols * sizeof(int));
        counter++;
    }

    int i = 0;
    int j = 0;
    while (i < rows)
    {
        j = 0;
        while (j < cols)
        {
            distance_matrix[i][j] = pow((stored_password[j] - stored_login[i]), 2);
            j++;
        }
        i++;
    }
    LOG_DBG("Calculation of distance matrix was a succes \n");
    return distance_matrix;
}

int **compute_accumulated_cost_matrix(char stored_password[], char stored_login[])
{
    // calc distance matrix
    int **dist_matrix = compute_euclidean_distance_matrix(stored_password, stored_login);
    LOG_DBG("printing dist matrix \n");
    print_matrix(dist_matrix);

    LOG_DBG("init cost_matrix on heap \n");
    int rows = SIZE;
    int cols = SIZE;
    // allocate cost matrix on heap
    int **cost_matrix = (int **)malloc(rows * sizeof(int *));
    int counter = 0;
    while (counter < rows)
    {
        cost_matrix[counter] = (int *)malloc(cols * sizeof(int));
        counter++;
    }

    // Init cost matrix
    LOG_DBG("init cost_matrix [0][0] \n");
    cost_matrix[0][0] = dist_matrix[0][0];

    int i = 0;
    while (i < SIZE)
    {
        cost_matrix[i][0] = dist_matrix[i][0] + cost_matrix[i - 1][0];
        i++;
    }

    int j = 0;
    while (j < SIZE)
    {
        cost_matrix[0][j] = dist_matrix[0][j] + cost_matrix[0][j - 1];
        j++;
    }

    i = 0;
    j = 0;

    while (i < SIZE)
    {
        j = 0;
        while (j < SIZE)
        {
            cost_matrix[i][j] = min(cost_matrix[i - 1][j],
                                    cost_matrix[i][j - 1],
                                    cost_matrix[i - 1][j - 1]) +
                                    dist_matrix[i][j];
            j++;
        }
        i++;
    }

    // Free mem for cost matrix
    free_matrix(cost_matrix);
    LOG_DBG("Calculationg cost matrix was a succes \n");
    return cost_matrix;
}


void print_matrix(int **matrix)
{
    int row = 0;
    while (row < SIZE)
    {
        int columns = 0;
        while (columns < SIZE)
        {
            printf("%d \t", matrix[row][columns]);
            columns++;
        }
        printf("\n");
        row++;
    }
}

int min(int cost1, int cost2, int cost3)
{
    if (cost1 <= cost2 && cost1 <= cost3)
    {
        return cost1;
    }
    else if (cost2 <= cost1 && cost2 <= cost3)
    {
        return cost2;
    }
    else
    {
        return cost3;
    }
}

int min_cost_warping_path(int **cost_matrix)
{

    int acum = 0;
    int row = 0;
    int colum = 0;

    while (row != SIZE - 1 && colum != SIZE-1)
    {
        /* defines moves operations */
        int horizontal = cost_matrix[row][colum + 1];
        int vertical   = cost_matrix[row + 1][colum];
        int diagonal   = cost_matrix[row + 1][colum + 1];

        // edge case
        if (colum == SIZE-1)
        {
            acum = acum + vertical;
            row = row + 1;
            printf("egde vertical %d-> \n", acum);
        }
        // edge case
        else if (row == SIZE - 1)
        {
            acum = acum + horizontal;
            colum = colum + 1;
            printf("egde horizontal %d-> \n", acum);
        }
        // horizontal case
        else if (horizontal < vertical && horizontal < diagonal)
        {
            acum = acum + horizontal;
            colum = colum + 1;
            printf("horizontal %d-> \n", acum);
        }
        // vertical case
        else if (vertical < horizontal && vertical < diagonal)
        {
            acum = acum + vertical;
            row = row + 1;
            printf("vertical %d-> \n", acum);
        }
        // diagonal case
        else if (diagonal < vertical && diagonal < horizontal)
        {
            acum = acum + diagonal;
            colum = colum + 1;
            row = row + 1;
            printf("diagonal %d-> \n", acum);
        }
        // diagonal prio case
        else if (diagonal == vertical || diagonal == horizontal)
        {
            acum = acum + diagonal;
            colum = colum + 1;
            row = row + 1;
            printf("diagonal prio %d-> \n", acum);
        }
         // vertical prio case
        else if (vertical == horizontal)
        {
            acum = acum + vertical;
            row = row + 1;
            printf("vertical prio %d-> \n", acum);
        }
        // Should never hit this
        else
        {
            printf("missing case \n");
        }
    }
    return acum;
}


void free_matrix(int** matrix)
{
    int i= 0;

    while(i < SIZE)
    {
        free(*(matrix + i));
        i++;
    }
    
    free(matrix);
}