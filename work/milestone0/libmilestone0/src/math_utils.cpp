#include <cstddef> //For size_t

#include "math_utils.h"


//Integrate a given polynomial function in three dimensions using trapezoidal approximation
double integrate_3d(Polynomial myfunc, double xstart, double xend, int xnum_points, 
                                       double ystart, double yend, int ynum_points, 
                                       double zstart, double zend, int znum_points)
{
    //result of integration
    double result = 0;
    
    //based on the number of points in each dimensions we define the delta-x/delta-y/delta-z
    double dx = (xend - xstart)/(double)xnum_points;
    double dy = (yend - ystart)/(double)ynum_points;
    double dz = (zend - zstart)/(double)znum_points;
    
    //first x-coordinate to start from
    double xcurr = xstart;
    
    //vector to hold the average function value in each step
    //This vector is like an array but automatically expands/shrinks to 
    // hold more/less values as necessary
    std::vector<double> average_function_value;
    
    //loop over the x-axis range
    for(int i = 0; i < xnum_points; i++)
    {
        //first y-coordinate to start from
        double ycurr = ystart;
        
        //loop over the y-axis range
        for(int j = 0; j < ynum_points; j++)
        {
            
            //first z-coordinate to start from
            double zcurr = zstart;
            
            //loop over the z-axis range
            for(int k = 0; k < znum_points; k++)
            {
                //array to store all the points that we need to average
                //we are averaging over the 8 vertices of this (dx,dy,dz) cube
                double *list_of_points = new double[8];
                
                //compute the function points that require averaging
                list_of_points[0] = myfunc.value(xcurr,ycurr,zcurr);
                list_of_points[1] = myfunc.value(xcurr+dx,ycurr,zcurr);
                list_of_points[2] = myfunc.value(xcurr,ycurr+dy,zcurr);
                list_of_points[3] = myfunc.value(xcurr+dx,ycurr+dy,zcurr);
                list_of_points[4] = myfunc.value(xcurr,ycurr,zcurr+dz);
                list_of_points[5] = myfunc.value(xcurr+dx,ycurr,zcurr+dz);
                list_of_points[6] = myfunc.value(xcurr,ycurr+dy,zcurr+dz);
                list_of_points[7] = myfunc.value(xcurr+dx,ycurr+dy,zcurr+dz);

                //compute the average of the function in our current delta-volume
                double favg = average(list_of_points,9);
                
                //push that average into a vector to be summed up later
                average_function_value.push_back(favg);
                
                //increment z coordinate
                zcurr += dz;
            }
            
            //increment y coordinate
            ycurr += dy;
        }
        
        //increment x coordinate
        xcurr += dx;
    }
    
    //loop over our average function values and perform approximate integration 
    // according to trapezoidal approximation
    for (size_t i = 0; i < average_function_value.size(); i++){
    
        //integral is summation of the product of our step (dx,dy,dz)
        // and the average function value in that space
        result += (average_function_value[i] * dx * dy * dz);
    }
    
    return result;
}

//computes the average over a vector of any size
double average(double *my_list_to_average, int num_elements)
{
    //variable to hold the summation over the array
    double total = 0;
    
    //loop over vector
    for(int i = 0; i <= num_elements; i++)
    {
        //increment total with current element
        total += my_list_to_average[i];
    }
    
    //average is the quotient of total and number of elements
    return total/(double)num_elements;
}
