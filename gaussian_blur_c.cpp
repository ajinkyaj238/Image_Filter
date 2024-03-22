#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono> 

using namespace std;

void print_matrix(int **mat, int size)
{
    for(int i = 0; i < size; i++ )
    {
        for(int j = 0; j < size; j++)
        {
            cout << mat[i][j] << ", "; 
        }
        cout << endl; 
    }
}



void write_matrix(int **mat, int size)
{
    std::ofstream file("output.txt");

    if (file.is_open()) {
        for(int i = 0; i < size; i++ )
        {
            for(int j = 0; j < size; j++)
            {
                file << mat[i][j];
                if (j != size - 1) {
                    file << ", ";
                }
            }
            file << "\n";
        }
        file.close();
    } else {
        std::cout << "Unable to open file";
    }
}



void gaussianBlur(int **image, double **kernel, int size, int kernel_size)
{
    auto start = std::chrono::high_resolution_clock::now();
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
            if ((row >= (size - 2)) && (col >= (size - 2)) )
            {
                cout << row << ", " << col << endl; 
                cout << sum<< " center" << endl; 
            }

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
                    if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                    if ((row >= (size - 2)) && (col >= (size - 2)) )
                    {
                        cout << sum << " up"<< endl; 
                    }
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
                    if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                    if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                        if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                        if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                        if ((row >= (size - 2)) && (col >= (size - 2)) )
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
                        if ((row >= (size - 2)) && (col >= (size - 2)) )
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
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";


    for(int i = 0; i < size; i++ )
    {
        for(int j = 0; j < size; j++)
        {
            image[i][j] = new_image[i][j];   
        }
    }


}



int main()
{

    // getting the size of the kernel
    int kernel_size; 
    std::ifstream file3("kernel_size.txt");
    if (!file3) {
        std::cout << "Unable to get kernel size";
        return 1; // exit if file not found
    }
    string line3; 
    while (std::getline(file3, line3)) {
        kernel_size = stoi(line3); 
    }
    file3.close();


    // getting the size of the image
    int size;
    std::ifstream file2("size.txt");
    if (!file2) {
        std::cout << "Unable to get size";
        return 1; // exit if file not found
    }
    string line2; 
    while (std::getline(file2, line2)) {
        size = stoi(line2);
    }
    file2.close(); 


    // defining the image
    int **image;
    image = new int*[size];
    for (int i = 0; i < size; ++i){
        image[i] = new int[size];
    }
    std::ifstream file("image.txt");
    if (!file) {
        std::cout << "Unable to get image";
        return 1; // exit if file not found
    }
    int row = 0; 
    string line;
    while (std::getline(file, line) && row < size) {
        std::istringstream iss(line);
        
        int col = 0;
        std::string token;
        while (iss >> token && col < size) {
            image[row][col] = std::stoi(token);
            ++col;
        }
        ++row;
    }
    file.close();




    // defining the dynamic kernel
    double **kernel;
    kernel = new double*[kernel_size];
    for (int i = 0; i < kernel_size; ++i) {
        kernel[i] = new double[kernel_size];
    }
    std::ifstream file1("kernel.txt");
    if (!file1){
        std::cout << "Unable to get kernel";
        return 1; // exit if file not found
    }
    string line1; 
    row = 0; 
    while (std::getline(file1, line1) && row < size) {
        std::istringstream iss(line1);
        
        int col = 0;
        std::string token;
        while (iss >> token && col < size) {
            kernel[row][col] = std::stod(token);
            ++col;
        }
        ++row;
    }
    file1.close();



    // for(int i = 0; i < kernel_size; i++){
    //     for(int j = 0; j < kernel_size; j++)
    //     {
    //         cout << kernel[i][j] << ", "; 
    //     }
    //     cout << endl; 
    // }

    // print_matrix(image, size);

    gaussianBlur(image, kernel, size, kernel_size);

    write_matrix(image, size); 





    return 0; 




}