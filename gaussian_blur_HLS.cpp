#include <fstream>
#include <string>
#include <sstream>

#include "ap_axi_sdata.h"
#include "hls_stream.h"

using namespace std;

#define M 256
#define K 5

void gaussianBlur(hls::stream< ap_axis<32, 2, 5, 6> >&image[M][M], hls::stream< ap_axis<32,2,5,6> >kernel[K][K])
{

    ap_axis<32, 2, 5, 6> tmp;

    #pragma HLS INTERFACE axis port=A
    #pragma HLS INTERFACE axis port=B
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=s_out


    int down, up, right, left, kernel_center, sum; 

    int **new_image; 
    new_image = new int*[size];
    for (int i = 0; i < size; ++i) {
        new_image[i] = new int[size];
    }

    down = up = right = left = sum = 0; 

    kernel_center = (kernel_size - 1) / 2; 


    for(int row = 0; row < size; row ++)
    {

        for(int col = 0; col < size; col++)
        {
            
            sum += (kernel[kernel_center][kernel_center] * image[row][col]);

            //check down
            if(row + 1 >= size)
            {
                down = 0; 
            }
            else
            {
                for(int i = kernel_center; i > 0; i--)
                {
                    if ((row + i) < size)
                    {
                        down = i; 
                        break; 
                    }
                }
                for(int i = 0; i < down; i++)
                {
                    sum += (kernel[kernel_center + i + 1][kernel_center] * image[row + i + 1][col]);
                }
            }


            //check up
            if(row - 1 < 0)
            {
                up = 0; 
            }
            else
            {
                for(int i = kernel_center; i > 0; i--)
                {
                    if ((row - i) >= 0)
                    {
                        up = i; 
                        break; 
                    }
                }
                for(int i = 0; i < up; i++)
                {
                    sum += (kernel[kernel_center - i - 1][kernel_center] * image[row - i - 1][col]);
                }
            }


            // check right
            if(col + 1 >= size)
            {
                right = 0; 
            }
            else
            {
                for(int i = kernel_center; i > 0; i--)
                {
                    if((col + i) < size)
                    {
                        right = i; 
                        break;
                    }
                }
                for(int i = 0; i < right; i++)
                {
                    sum += (kernel[kernel_center][kernel_center + i + 1] * image[row][col + i + 1]);
                }
            }            


            //check left
            if(col - 1 < 0)
            {
                left = 0; 
            }
            else
            {
                for(int i = kernel_center; i > 0; i--)
                {
                    if((col - i) >= 0)
                    {
                        left = i; 
                        break; 
                    }
                }
                for(int i = 0; i < left; i++)
                {
                    sum += (kernel[kernel_center][kernel_center + i + 1] * image[row][col + i + 1]);
                }
            }


            // bottom right square 
            if ((down > 0) && (right > 0))
            {
                for(int br_h = 0; br_h < down; br_h++)
                {
                    for(int br_w = 0; br_w < right; br_w++)
                    {
                        sum += (kernel[kernel_center + br_h + 1][kernel_center + br_w + 1] * image[row + br_h + 1][col + br_w + 1]);
                    }
                }
            }


            // bottom left square (different variable loops are used for loop fission)
            if ((down > 0) && (left > 0))
            {
                for(int bl_h = 0; bl_h < down; bl_h++)
                {
                    for(int bl_w = 0; bl_w < left; bl_w++)
                    {
                        sum += (kernel[kernel_center + bl_h + 1][kernel_center - bl_w - 1] * image[row + bl_h + 1][col - bl_w - 1]);
                    }
                }
            }


            // top right square (different variable loops are used for loop fission)
            if ((up > 0) && (right > 0))
            {
                for(int tr_h = 0; tr_h < up; tr_h++)
                {
                    for(int tr_w = 0; tr_w < right; tr_w++)
                    {
                        sum += (kernel[kernel_center - tr_h - 1][kernel_center + tr_w + 1] * image[row - tr_h - 1][col + tr_w + 1]);
                    }
                }
            }


            // top left square (different variable loops are used for loop fission)
            if ((up > 0) && (left > 0))
            {
                for(int tl_h = 0; tl_h < up; tl_h++)
                {
                    for(int tl_w = 0; tl_w < left; tl_w++)
                    {
                        sum += (kernel[kernel_center - tl_h - 1][kernel_center - tl_w - 1] * image[row - tl_h - 1][col - tl_w - 1]);
                    }
                }
            }




            new_image[row][col] = sum;
            sum = 0; 
        } 
    }    

    for(int i = 0; i < size; i++ )
    {
        for(int j = 0; j < size; j++)
        {
            image[i][j] = new_image[i][j];   
        }
    }


}