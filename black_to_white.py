import cv2

img = cv2.imread('results/00000.png')

(row, col, channels) = img.shape

for r in range(row):
    for c in range(col):
        px = img[r][c]
        if all(px == 0):
            img[r][c] = 255

cv2.imwrite('results/00000_bw.png', img)