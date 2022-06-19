from re import I
import cv2
from cv2 import equalizeHist
from cv2 import IMREAD_COLOR
import numpy as np
# https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/


def path(num):
    if (num < 10):
        path = '000' + str(num)
    elif(num >= 10 and num < 100):
        path = '00' + str(num)
    elif(num >= 100 and num < 1000):
        path = '0' + str(num)
    else:
        path = str(num)
    return path


i = 1
k = 1
while i < 1067:

    img0 = cv2.imread('../../test_fd/20220407T144859/gss_camera/' + path(i) + '.png', IMREAD_COLOR)
    #img0 = cv2.equalizeHist(img0)
    '''
    img1 = cv2.imread('../../60fps_1cd/gss_camera/' + path(i - 1) + '.png')
    img2 = cv2.imread('../../60fps_1cd/gss_camera/' + path(i - 2) + '.png')
    img3 = cv2.imread('../../60fps_1cd/gss_camera/' + path(i - 3) + '.png')
    img4 = cv2.imread('../../60fps_1cd/gss_camera/' + path(i - 4) + '.png')
    '''
    '''
    if i >= 95:
        kernel_motion_blur = np.zeros((k, k))
        kernel_motion_blur[:, int((k-1)/2)] = np.ones(k)
        kernel_motion_blur = kernel_motion_blur / k
        #cv2.imshow('Kernel Blur', kernel_motion_blur)
        # applying the kernel to the input imag
        output = cv2.filter2D(img0, -1, kernel_motion_blur)
        #cv2.imshow('Motion Blur', output)
        k = k + 1
    else:
        output = img0
        '''
    '''
    img0 = img0 * 0.45
    img1 = img1 * 0.3
    img2 = img2 * 0.2
    img3 = img3 * 0.05
    '''
    '''
    img0 = img0 * 0.05
    img1 = output * 0.95
    img = cv2.add(img0, img1)
    #img = cv2.add(img, img2, img)
    #img = cv2.add(img, img3, img)
'''

    scale_percent = 60  # percent of original size
    width = int(img0.shape[1] * scale_percent / 100)
    height = int(img0.shape[0] * scale_percent / 100)
    dim = (width, height)
    img0 = cv2.resize(img0, dim)

    file_name = '../../test_sine_60fps/frame_' + str(i) + '.png'
    cv2.imwrite(file_name, img0)

    
    #cv2.imshow('fqewf', img0)
    #cv2.waitKey(0)
    

    print(i)
    i = i + 1
