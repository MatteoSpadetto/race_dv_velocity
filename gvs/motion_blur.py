import cv2
import numpy as np

img = cv2.imread('./test_0/20220407T144859/gss_camera/0900.png')
#https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/

scale_percent = 60  # percent of original size
width = int(img.shape[1] * scale_percent / 100)
height = int(img.shape[0] * scale_percent / 100)
dim = (width, height)

img = cv2.resize(img, dim)
cv2.imshow('Original', img)

size = 50

# generating the kernel
kernel_motion_blur = np.zeros((size, size))
kernel_motion_blur[:, int((size-1)/2)] = np.ones(size)
kernel_motion_blur = kernel_motion_blur / size

# applying the kernel to the input image
output = cv2.filter2D(img, -1, kernel_motion_blur)

cv2.imshow('Motion Blur', output)
cv2.imwrite('mb_0.png', output)
cv2.waitKey(0)
