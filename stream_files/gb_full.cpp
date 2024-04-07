#include "ap_axi_sdata.h"
#include "hls_stream.h"

#define SIZE 512 //size of the overlapped window and the kernel size
#define WINDOW_SIZE 5 //size of the kernel
typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi




void gaussianBlur(
		hls::stream<std_axi>&img,
		hls::stream<std_axi>&out_pix
)
{
	// convert image input stream array into regular array
	uint8_t image[SIZE * SIZE];
	std_axi tmp;
	for(int i = 0; i < SIZE * SIZE; i++)
	{
		img.read(tmp);
		image[i] = tmp.data.to_int();
	}


	findWindow(image);
}







void windowOperation(


)
{

#pragma HLS INTERFACE axis port=window
#pragma HLS INTERFACE axis port=out_pix
#pragma hls interface s_axilite port=return

	std_axi window_tmp;
	float sum = 0;
	float kernel[WINDOW_SIZE][WINDOW_SIZE] = {
			{0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497},
			{0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651},
			{0.021938231279714643, 0.09832033134884577, 0.16210282163712664, 0.09832033134884577, 0.021938231279714643},
			{0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651},
			{0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497}
	}; //5x5 kernel


	for(int i = 0; i < WINDOW_SIZE; i++)
	{
		for(int j = 0; j < WINDOW_SIZE; j++)
		{
			window.read(window_tmp);
			sum += window_tmp.data.to_float() * kernel[i][j]; //both to float to prevent any type error

			if(window_tmp.last){
				window_tmp.data = sum;
				out_pix.write(window_tmp);
				break;
			}
		}
	}

}









void findWindow(int *image)
{

    int down, up, right, left, kernel_center, sum;
    down = up = right = left = sum = 0;
    kernel_center = (WINDOW_SIZE - 1) / 2;


    for(int row = 0; row < SIZE; row ++)
    {

        for(int col = 0; col < SIZE; col++)
        {

            sum += (kernel[kernel_center][kernel_center] * image[row][col]);
            if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
            {
                cout << row << ", " << col << endl;
                cout << sum<< " center" << endl;
            }

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
                    sum += (kernel[kernel_center + i + 1][kernel_center] * image[row + i + 1][col]);
                    if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                    {
                        cout << sum << " down"<< endl;
                    }
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
                    if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                    {
                        cout << sum << " up"<< endl;
                    }
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
                    sum += (kernel[kernel_center][kernel_center + i + 1] * image[row][col + i + 1]);
                    if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                    {
                        cout << sum << " right"<< endl;
                    }
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
                    if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                    {
                        cout << sum << " left"<< endl;
                    }
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
                        if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                        {
                            cout << sum << " bottom right"<< endl;
                        }
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
                        if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                        {
                            cout << sum << " bottom left"<< endl;
                        }
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
                        if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                        {
                            cout << sum << " top right"<< endl;
                        }
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
                        if ((row >= (SIZE - 2)) && (col >= (SIZE - 2)) )
                        {
                            cout << sum << " top left"<< endl;
                        }
                    }
                }
            }




            new_image[row][col] = sum;
            cout<< "Pixel " << row+1 << ", " << col+1 << " finished; val = " << sum << endl;
            sum = 0;
        }
    }


}
