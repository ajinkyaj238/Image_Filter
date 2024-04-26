#include "ap_axi_sdata.h"
#include "hls_stream.h"
//#include "stdint.h"
#include "ap_int.h"




#define SIZE 128 //size of the overlapped window and the kernel size
#define WINDOW_SIZE 5 //size of the kernel and the multiplied window
#define SIZE_LEN (SIZE * SIZE) // size of the image array

typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi





void gaussianBlur(hls::stream<std_axi>&img, hls::stream<std_axi>&out_pix)
{


    #pragma HLS INTERFACE axis port=img
    #pragma HLS INTERFACE axis port=out_pix
    #pragma hls interface s_axilite port=return


    // all variables initialized
    int down, up, right, left, kernel_center;
    float sum;
    down = up = right = left = sum = 0;
    kernel_center = ((WINDOW_SIZE) - 1) / 2;


    float kernel[WINDOW_SIZE][WINDOW_SIZE] = {
        {0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497},
        {0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651}, 
        {0.021938231279714643, 0.09832033134884577, 0.16210282163712664, 0.09832033134884577, 0.021938231279714643}, 
        {0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651},
        {0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497}
    };


    // Stream data in
    std_axi tmp1, tmp2;
    ap_int<8> tmp_conv;
    ap_int<8> image[SIZE][SIZE];


    while(1)
    {
        for(int i = 0; i < SIZE; i++)
        {
            for(int j =0; j < SIZE; j++)
            {
                img.read(tmp1);
                if(tmp1.last)
        		{
        			break;
        		}
                image[i][j] = (ap_int<8>)tmp1.data.to_int();
            }
        }


    }


        //creates a new image array with all values as 0
        ap_int<8> new_image[SIZE][SIZE];


        for(int row = 0; row < (SIZE); row++)
        {
            for(int col = 0; col < (SIZE); col++)
            {
#pragma HLS PIPELINE off


                sum = 0;

                //center
                sum += ( kernel[kernel_center][kernel_center] * (int)image[row][col]);




                //check down
                if(row + 1 >= SIZE)
                {
                    down = 0;
                }
                else
                {
                    for(int i = kernel_center; i > 0; i--)
                    {
                        if ((row + i) < SIZE)
                        {
                            down = i;
                            break;
                        }
                    }
                    for(int i = 0; i < down; i++)
                    {
                        sum += (kernel[kernel_center + i + 1][kernel_center] * (int)image[row + i + 1][col] );
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
                        sum += (kernel[kernel_center - i - 1][kernel_center] * (int)image[row - i - 1][col] );
                    }
                }






                // check right
                if(col + 1 >= SIZE)
                {
                    right = 0;
                }
                else
                {
                    for(int i = kernel_center; i > 0; i--)
                    {
                        if((col + i) < SIZE)
                        {
                            right = i;
                            break;
                        }
                    }
                    for(int i = 0; i < right; i++)
                    {
                        sum += (kernel[kernel_center][kernel_center + i + 1] * (int)image[row][col + i + 1] );
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
                        sum += (kernel[kernel_center][kernel_center + i + 1] * (int)image[row][col + i + 1] );
                    }
                }





                // bottom right square
                if ((down > 0) && (right > 0))
                {
                    for(int i = 0; i < down; i++)
                    {
                        for(int j = 0; j < right; j++)
                        {
                            sum += (kernel[kernel_center + i + 1][kernel_center + j + 1] * (int)image[row + i + 1][col + j + 1] );
                        }
                    }
                }




                // bottom left square (different variable loops are used for loop fission)
                if ((down > 0) && (left > 0))
                {
                    for(int i = 0; i < down; i++)
                    {
                        for(int j = 0; j < left; j++)
                        {
                            sum += (kernel[kernel_center + i + 1][kernel_center - j - 1] * (int)image[row + i + 1][col - j - 1] );
                        }
                    }
                }





                // top right square (different variable loops are used for loop fission)
                if ((up > 0) && (right > 0))
                {
                    for(int i = 0; i < up; i++)
                    {
                        for(int j = 0; j < right; j++)
                        {
                            sum += (kernel[kernel_center - i - 1][kernel_center + j + 1] * (int)image[row - i - 1][col + j + 1]) ;
                        }
                    }
                }





                // top left square (different variable loops are used for loop fission)
                if ((up > 0) && (left > 0))
                {
                    for(int i = 0; i < up; i++)
                    {
                        for(int j = 0; j < left; j++)
                        {
                            sum += (kernel[kernel_center - i - 1][kernel_center - j - 1] * (int)image[row - i - 1][col - j - 1]) ;
                        }
                    }
                }






                new_image[row][col] = (ap_int<8>)sum;




            }
        }


    while(1){

    	tmp2.last = 0;

        // simulating output stream
        for(int i = 0; i<SIZE; i++)
        {
            for(int j = 0; j<SIZE; j++)
            {
                tmp2.data = (int)new_image[i][j];
                out_pix.write(tmp2);
            }
        }

        break;
    }


    tmp2.last = 1;
    out_pix.write(tmp2);

}
