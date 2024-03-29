#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define size 512 //size of the image
#define kernel_size 5 // size of the kernel

typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi

void gaussianBlur(
		hls::stream<std_axi>&image,
		hls::stream<std_axi>&new_image,
		hls::stream<std_axi>&kernel
		)
{

	#pragma HLS INTERFACE axis port=image
	#pragma HLS INTERFACE axis port=new_image
	#pragma HLS INTERFACE axis port=kernel
	#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS



	std_axi curr_pixel;



	int image_buff[size][size];
	int kernel_buff[kernel_size][kernel_size];
	int newimage_buff[size][size];



	// Reading image into the buffer:
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			curr_pixel = image.read();
			image_buff[i][j] = curr_pixel.data;
		}
	}


	// Reading kernel into the buffer
	for(int i = 0; i < kernel_size; i++)
	{
		for(int j = 0; j < kernel_size; j++)
		{
			curr_pixel = image.read();
			kernel_buff[i][j] = curr_pixel.data;
		}
	}


    int down, up, right, left, kernel_center, sum;  
    down = up = right = left = sum = 0; 
    kernel_center = (kernel_size - 1) / 2; 


    for(int row = 0; row < size; row ++) // iterates through the rows
    {
		#pragma HLS PIPELINE

        for(int col = 0; col < size; col++) // iterates through the columns
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
}
