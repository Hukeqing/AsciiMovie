# import cv2
from cv2 import VideoCapture, imwrite, waitKey
import sys

vc = VideoCapture(sys.argv[1])  # 读入视频文件
c = 0
rval = vc.isOpened()
while rval:
    c = c + 1
    rval, frame = vc.read()
    if rval:
        imwrite(sys.argv[2] + '/' + str(c) + '.jpg', frame)  # 存储为图像
        waitKey(1)
    else:
        break
vc.release()
with open(sys.argv[2] + '/cnt.txt', 'w') as f:
    f.write(str(c))
