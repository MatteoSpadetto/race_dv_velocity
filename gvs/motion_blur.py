from re import I
import cv2
from cv2 import imshow
from cv2 import circle
import numpy as np

img = cv2.imread('../../test_fd/20220407T144859/gss_camera/0900.png')
# https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/


for i in range(0, img.shape[1]):
    if (i % 50 == 0):
        for j in range(0, img.shape[0]):
            if (j % 50 == 0):
                cv2.circle(img, (i, j), 1, (255, 255, 255), 2)


'''
cv2.circle(img, (900, 500), 20, (255, 255, 255), 40)
'''

scale_percent = 60  # percent of original size
width = int(img.shape[1] * scale_percent / 100)
height = int(img.shape[0] * scale_percent / 100)
dim = (width, height)


img = cv2.resize(img, dim)
#cv2.imshow('Original', img)


i = 1
while i < 120:
    size = i * 1

    # generating the kernel
    kernel_motion_blur = np.zeros((size, size))
    kernel_motion_blur[:, int((size-1)/2)] = np.ones(size)
    kernel_motion_blur = kernel_motion_blur / size
    #cv2.imshow('Kernel Blur', kernel_motion_blur)

    # applying the kernel to the input image
    output = cv2.filter2D(img, -1, kernel_motion_blur)
    #cv2.imshow('Motion Blur', output)
    file_name = "../../test_sp/mb_speed_" + str(i) + ".png"
    cv2.imwrite(file_name, output)
    i = i + 1

#cv2.waitKey(0)
