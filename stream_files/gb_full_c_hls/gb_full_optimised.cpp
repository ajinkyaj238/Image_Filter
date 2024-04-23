#include "ap_axi_sdata.h"
#include "hls_stream.h"
#include "stdint.h"
//#include "ap_int.h"


#define SIZE 512 //size of the overlapped window and the kernel size
#define SIZE_LEN (SIZE * SIZE) // size of the image array
#define WINDOW_SIZE 5 //size of the kernel and the multiplied window
#define WINDOW_LEN (WINDOW_SIZE * WINDOW_SIZE) // size of the window array



typedef ap_axis<32,2,5,6> std_axi; //renaming as standard axi or std_axi


/*
Multiplies the window and the kernel floats together and saves the value in the first index of window
*/
void windowOperation(int *window)
{

#pragma HLS pipeline off

	float sum = 0;
	//the 5x5 kernel
	float kernel[WINDOW_LEN] = {0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497, 0.013306209891013651, 0.05963429543618014, 0.09832033134884577, 0.05963429543618014, 0.013306209891013651, 0.021938231279714643, 0.09832033134884577, 0.16210282163712664, 0.09832033134884577, 0.021938231279714643, 0.002969016743950497, 0.013306209891013651, 0.021938231279714643, 0.013306209891013651, 0.002969016743950497};

	for(int i = 0; i < WINDOW_SIZE; i++){
        sum += window[i] * kernel[i];
    }

    window[0] = sum;
}





void gaussianBlur(hls::stream<std_axi>&img, hls::stream<std_axi>&out_pix)
{

#pragma HLS INTERFACE axis port=img
#pragma HLS INTERFACE axis port=out_pix
#pragma hls interface s_axilite port=return

	// convert image input stream array into regular array
	uint8_t image[SIZE_LEN];
	std_axi tmp;
	for(int i = 0; i < SIZE_LEN; i++)
	{
		img.read(tmp);
		image[i] = tmp.data.to_int();
	}


	// computing and sending back out_pix for each pixel
	int down, up, right, left, kernel_center, edge_counter;
	down = up = right = left = edge_counter = 0;
	int window[WINDOW_LEN];
	kernel_center = ((WINDOW_LEN) - 1) / 2;

	for(int pix = 0; pix < (SIZE_LEN); pix++)
	{
		edge_counter += 1; // checks if the pixel is at the last column of the row
		if (edge_counter == SIZE)
		{
			edge_counter = 0;
		}

		//presetting all the values to 0 for the window
		for(int i = 0; i < WINDOW_LEN; i++){
			window[i] = 0;
		}


		// center
		window[kernel_center] = image[pix];


		// down
		if(pix + SIZE >= SIZE_LEN){
			down = 0;
		}
		else{ // checks how many positions down exist

			for(int i = kernel_center; i > 0; i--){

				// if the arb_row from the current pixel is still within the array
				if( (pix + (i * SIZE)) < SIZE_LEN)
				{
					down = i;
				}
			}
			for(int i = 1; i < down + 1; i++){
				window[kernel_center + (i*WINDOW_SIZE)] = image[pix + (i*SIZE)];
			}

		}


		// up
		if(pix - SIZE < 0){
			up = 0;
		}
		else{ // checks how many positions down exist

			for(int i = kernel_center; i > 0; i--){

				// if the arb_row from the current pixel is still within the array
				if( (pix - (i * SIZE)) >= 0)
				{
					up = i;
				}
			}
			for(int i = 1; i < up + 1; i++){
				window[kernel_center - (i*WINDOW_SIZE)] = image[pix - (i*SIZE)];
			}

		}


		// right
		if(edge_counter + 1 >= SIZE){
			right = 0;
		}
		else{ // checks how many positions down exist

			for(int i = kernel_center; i > 0; i--){

				// if the arb_row from the current pixel is still within the array
				if( edge_counter + i < SIZE)
				{
					right = i;
				}
			}
			for(int i = 1; i < right + 1; i++){
				window[kernel_center + i] = image[pix + i];
			}

		}


		// left
		if(edge_counter - 1 < 0){
			left = 0;
		}
		else{ // checks how many positions down exist

			for(int i = kernel_center; i > 0; i--){

				// if the arb_row from the current pixel is still within the array
				if(edge_counter - i >= SIZE)
				{
					left = i;
				}
			}
			for(int i = 1; i < right + 1; i++){
				window[kernel_center - i] = image[pix - i];
			}

		}


		// bottom right square
		if ((down > 0) && (right > 0)){
			for(int i = 1; i < down + 1; i++)
			{
				for(int j = 1; j < right + 1; j++)
				{
					window[kernel_center + j + (i*WINDOW_SIZE)] = image[pix + j + (j*SIZE)];
				}
			}
		}


		//bottom left square
		if ((down > 0) && (left > 0)){
			for(int i = 1; i < down + 1; i++)
			{
				for(int j = 1; j < left + 1; j++)
				{
					window[kernel_center + (i*WINDOW_SIZE) - j] = image[pix + (j*SIZE) - j];
				}
			}
		}


		//top right square
		if ((up > 0) && (right > 0)){
			for(int i = 1; i < up + 1; i++)
			{
				for(int j = 1; j < right + 1; j++)
				{
					window[kernel_center + j - (i*WINDOW_SIZE)] = image[pix + j - (j*SIZE)];
				}
			}
		}


		//top left square
		if((up > 0) && (left > 0)){
			for(int i = 1; i < up + 1; i++)
			{
				for(int j = 1; j < left + 1; j++)
				{
					window[kernel_center - j - (i*WINDOW_SIZE)] = image[pix - j - (j*SIZE)];
				}
			}
		}

		windowOperation(window); // the new pixel value gets saved in the first index of window array

		tmp.data = window[0];
		out_pix.write(tmp);
	}


}







