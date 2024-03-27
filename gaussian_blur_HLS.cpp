#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define size 512 //size of the image
#define kernel_size 5 // size of the kernel

void gaussianBlur(
		volatile int *image,
		volatile int *kernel,
		volatile int *newimage
)
{

	#pragma HLS INTERFACE m_axi port = image offset = slave bundle = image_port
	#pragma HLS INTERFACE m_axi port = kernel offset = slave bundle = kernel_port
	#pragma HLS INTERFACE m_axi port = newimage offset = slave bundle = newimage_port
	#pragma HLS INTERFACE s_axilite port = return bundle = CONTROL_BUS

	int image_buff[size][size];
	int kernel_buff[kernel_size][kernel_size];
	int newimage_buff[size][size];


	memcpy(&image_buff[0][0], const_cast<int*>(image), sizeof(int) * size * size);
	memcpy(&kernel_buff[0][0], const_cast<int*>(kernel), sizeof(int) * kernel_size * kernel_size);


    int down, up, right, left, kernel_center, sum;  


    down = up = right = left = sum = 0; 

    kernel_center = (kernel_size - 1) / 2; 


    for(int row = 0; row < size; row ++)
    {

        for(int col = 0; col < size; col++)
        {
            
            sum += (kernel_buff[kernel_center][kernel_center] * image_buff[row][col]);

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
                    sum += (kernel_buff[kernel_center + i + 1][kernel_center] * image_buff[row + i + 1][col]);
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
                    sum += (kernel_buff[kernel_center - i - 1][kernel_center] * image_buff[row - i - 1][col]);
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
                    sum += (kernel_buff[kernel_center][kernel_center + i + 1] * image_buff[row][col + i + 1]);
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
                    sum += (kernel_buff[kernel_center][kernel_center + i + 1] * image_buff[row][col + i + 1]);
                }
            }


            // bottom right square 
            if ((down > 0) && (right > 0))
            {
                for(int i = 0; i < down; i++)
                {
                    for(int j = 0; j < right; j++)
                    {
                        sum += (kernel_buff[kernel_center + i + 1][kernel_center + j + 1] * image_buff[row + i + 1][col + j + 1]);
                    }
                }
            }


            // bottom left square
            if ((down > 0) && (left > 0))
            {
                for(int i = 0; i < down; i++)
                {
                    for(int j = 0; j < left; j++)
                    {
                        sum += (kernel_buff[kernel_center + i + 1][kernel_center - j - 1] * image_buff[row + i + 1][col - j - 1]);
                    }
                }
            }


            // top right square
            if ((up > 0) && (right > 0))
            {
                for(int i = 0; i < up; i++)
                {
                    for(int j = 0; j < right; j++)
                    {
                        sum += (kernel_buff[kernel_center - i - 1][kernel_center + j + 1] * image_buff[row - i - 1][col + j + 1]);
                    }
                }
            }


            // top left square
            if ((up > 0) && (left > 0))
            {
                for(int i = 0; i < up; i++)
                {
                    for(int j = 0; j < left; j++)
                    {
                        sum += (kernel_buff[kernel_center - i - 1][kernel_center - j - 1] * image_buff[row - i - 1][col - j - 1]);
                    }
                }
            }

            newimage_buff[row][col] = sum;
            sum = 0;

        } 
    }

    memcpy(const_cast<int*>(newimage), const_cast<int*>(&newimage_buff[0][0]), sizeof(int) * size * size);


}
