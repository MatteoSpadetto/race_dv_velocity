from re import I
import cv2
from cv2 import imshow
from cv2 import circle
from cv2 import waitKey
import numpy as np
# https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/


i = 90
k = 1
while i < 300:
    if (i < 10):
        path = '000' + str(i)
    elif(i >= 10 and i < 100):
        path = '00' + str(i)
    elif(i >= 100 and i < 1000):
        path = '0' + str(i)
    else:
        path = str(i)
    print(path)

    img = cv2.imread(
        '../../test_fd/20220407T144859/gss_camera/' + path + '.png')

    scale_percent = 60  # percent of original size
    width = int(img.shape[1] * scale_percent / 100)
    height = int(img.shape[0] * scale_percent / 100)
    dim = (width, height)
    img = cv2.resize(img, dim)
    output = img

    if i >= 90 + 20:
        kernel_motion_blur = np.zeros((k, k))
        kernel_motion_blur[:, int((k-1)/2)] = np.ones(k)
        kernel_motion_blur = kernel_motion_blur / k
        #cv2.imshow('Kernel Blur', kernel_motion_blur)
        # applying the kernel to the input image
        output = cv2.filter2D(img, -1, kernel_motion_blur)
        #cv2.imshow('Motion Blur', output)
        k = k + 1

    file_name = "../../test_speed_v1/mb_speed_" + str(i-89) + ".png"
    cv2.imwrite(file_name, output)

    cv2.imshow('fqewf', output)
    cv2.waitKey(10)
    i = i + 1
