from PIL import Image
import os
import re
import numpy as np
from scipy import misc

def get_dirs(path):
    if os.path.exists(path):
        return os.listdir(path)
    else:
        return -1

def rgb2gray(pixel):
    return np.dot(pixel,[0.299,0.587,0.144])

def resize_image(gray):

    current_path = os.getcwd()
    subpath = raw_input('Enter the goal path---->')
    path = current_path + '/' + subpath
    files = get_dirs(path)

    prefix_name =raw_input("Enter the file's prefix(E.g. door_1.jpg->prefix is door)---->")

    test_image_path = path + '/' + files[1]
    test_im = Image.open(test_image_path)
    print 'Size of image sample(Width x Height):',test_im.size

    width_input = raw_input('Enter the width that is you want resize---->')
    image_width = int(width_input)

    height_input = raw_input('Enter the height that is you want resize---->')
    image_height = int(height_input)

    image_type = '.' +raw_input('Enter the type of image---->')


    if len(files) !=0:


        for f in files:
            ext = os.path.splitext(f)[1]
            if (ext == image_type):
                image_input = path + '/' + f


                im = Image.open(image_input)
                (x,y) = im.size
                #image_height = y * image_width / x

                image_outname = os.path.splitext(f)[0] + '_'+width_input+'x'+str(image_height) + os.path.splitext(f)[1]
                image_output = path + '/' + image_outname
                #image_outname_2 = os.path.splitext(f)[0] + '_'+width_input+'x'+str(image_height) + '.bmp'
                #image_output_2 = path + '/' + image_outname_2

                out = im.resize((image_width, image_height), Image.ANTIALIAS)
                if (gray==1):
                    (x_gray,y_gray) = out.size
                    out_temp = np.zeros((x_gray,y_gray))
                    out = out.load()
                    for i in range(x_gray):
                        for j in range(y_gray):
                            out_temp[i,j] = rgb2gray(out[i,j])
                    out_temp = out_temp.T
                    out = Image.fromarray(out_temp).convert('L')

                    image_outname = os.path.splitext(f)[0] + '_'+width_input+'x'+str(image_height)+ '_grey' + os.path.splitext(f)[1]
                    image_output = path + '/' + image_outname

                    out.save(image_output)
                else:
                    out.save(image_output)
            else:
                continue

if __name__ == '__main__':
    gray_convert = raw_input('If you want to convert the image to GRAY scale? Yes->enter 1; No->enter other num--->')
    gray = int(gray_convert)
    resize_image(gray)

