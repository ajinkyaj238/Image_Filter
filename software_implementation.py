import cv2 
from matplotlib import pyplot as plt
import numpy as np
import math


# Creates the kernels
def getKernels(size, sigma = 1):
    kernel = [[0] * size for _ in range(size)]
    center = size // 2

    total = 0
    for x in range(size):
        for y in range(size):
            kernel[x][y] = math.exp(-((x - center)**2 + (y - center)**2) / (2 * sigma**2))
            total += kernel[x][y]

    # Normalize the kernel
    for x in range(size):
        for y in range(size):
            kernel[x][y] /= total

    print(kernel)

    return kernel



"""
Returns: A dicitonary with the height, width, 
"""
def importImages(name):

    image_info = {}
    
    # original_image = cv2.imread('cameraman.jpg')
    noisy_image = cv2.imread(name)


    # cv2.imshow('Image1', original_image)
    cv2.imshow('Input', noisy_image)

    height, width, channels = noisy_image.shape

    cv2.waitKey(0)
    cv2.destroyAllWindows()


    image_info['height'] = height
    image_info['width'] = width
    image_info['channels'] = channels
    image_info['image'] = noisy_image # list of list image values


    noisy_image = cv2.cvtColor(noisy_image, cv2.COLOR_BGR2GRAY)
    image_info['bw_image'] = noisy_image #single string image
    
    return image_info


def gaussianBlur(image, kernel):
        
    down = 0 #determines what the kernel can compute. 
    up = 0
    right = 0 
    left = 0
    kernel_center = int((len(kernel) - 1)/2)

    new_image = []
    for row in range(len(image)):

        new_row = []

        for col in image[row]:
            
            # reinitializes the value of the sum to 0
            sum = 0

            # center
            sum =  sum + (kernel[kernel_center][kernel_center] * image[row][col])

            # check down
            if((row + 1) >= len(image)):
                down = 0 
            else:
                for i in range(kernel_center, 0-1, -1):
                    if (row + i < len(image)):
                        down = i
                        break
                for i in range(down):
                    sum = sum + (kernel[kernel_center + i + 1][kernel_center] * image[row + i + 1][col])
                

            # check up
            if((row - 1) < 0):
                up = 0
            else:
                for i in range(kernel_center, 0-1, -1):
                    if (row - i >= 0):
                        up = i
                        break
                for i in range(up):
                    sum = sum + (kernel[kernel_center - i - 1][kernel_center] * image[row - i - 1][col])


            # check right
            if((col+1) >= len(image[row])):
                right = 0
            else:
                for i in range(kernel_center, 0-1, -1):
                    if (col + i < len(image[row])):
                        right = i
                        break
                for i in range(right):
                    sum = sum + (kernel[kernel_center][kernel_center + i + 1] * image[row][col + i + 1])


            # check left
            if((col-1) < 0):
                left = 0
            else:
                for i in range(kernel_center, 0-1, -1):
                    if((col - i) >= 0):
                        left = i
                        break
                for i in range(left):
                    sum = sum + (kernel[kernel_center][kernel_center - i - 1] * image[row][col - i - 1])


            #bottom right square 
            if (down > 0) and (right > 0):
                for i in range(down):
                    for j in range(right):
                        sum = sum + (kernel[kernel_center + i + 1][kernel_center + j + 1] * image[row + i + 1][col + j + 1])


            #bottom left square 
            if (down > 0) and (left > 0):
                for i in range(down):
                    for j in range(left):
                        sum = sum + (kernel[kernel_center + i + 1][kernel_center - j - 1] * image[row + i + 1][col - j - 1])


            #top right square 
            if (up > 0) and (right > 0):
                for i in range(up):
                    for j in range(right):
                        sum = sum + (kernel[kernel_center - i - 1][kernel_center + j + 1] * image[row - i - 1][col + j + 1])


            #top left square 
            if (up > 0) and (left > 0):
                for i in range(up):
                    for j in range(left):
                        sum = sum + (kernel[kernel_center - i - 1][kernel_center - j - 1] * image[row - i - 1][col - j - 1])


            
            new_row.append(sum) # per column
            print("pixel " + str(row) + " " + str(col) + " complete")


        new_image.append(new_row) # per row

    return new_image


def buildImage(pixel_values, dimension, original_image, bw_image):

    image_array = np.array(pixel_values).reshape((dimension, dimension))

    image_array = np.uint8(image_array)

    # Create the image using cv2
    final_image = cv2.resize(image_array, (dimension, dimension), interpolation=cv2.INTER_NEAREST)

    # Display the image
    # cv2.imshow('original input', original_image)
    cv2.imshow('black and white input', bw_image)
    cv2.imshow('Output', final_image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    return final_image





if __name__ == '__main__':

    
    test_image = []
    for i in range(256):
        test_image_row = []
        for j in range(256):
            test_image_row.append(50)
        test_image.append(test_image_row)
    np.set_printoptions(threshold=np.inf)



    # grabs images
    image_info = importImages('cameraman.jpg')

    image_info['kernel'] = getKernels(11, 1)

    # image_info['kernel'] = [[0.003663003663003663, 0.014652014652014652, 0.02564102564102564, 0.014652014652014652, 0.003663003663003663], [0.014652014652014652, 0.05860805860805861, 0.09523809523809523, 0.05860805860805861, 0.014652014652014652], [0.02564102564102564, 0.09523809523809523, 0.15018315018315018, 0.09523809523809523, 0.02564102564102564], [0.003663003663003663, 0.014652014652014652, 0.02564102564102564, 0.014652014652014652, 0.003663003663003663], [0.014652014652014652, 0.05860805860805861, 0.09523809523809523, 0.05860805860805861, 0.014652014652014652]]
    # image_info['kernel'] = [[0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612], [0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612, 0.02040816326530612]]

    #Gaussian Blur Test
    image_info['gaussian_blur'] = gaussianBlur(image_info['bw_image'], image_info['kernel'])


    #Bilateral Filter. 


    #builds black and white image
    image_info['final_image'] = buildImage(image_info['gaussian_blur'], image_info['height'], image_info['image'], image_info['bw_image'])




#bulding the image

