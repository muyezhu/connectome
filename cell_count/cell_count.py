import sys
import cv2
import numpy as np
import scipy.ndimage as spimg
import scipy.misc
# input images: fg_lat (35x35+7%) => => dilate => fg_lat_dilate (bg)              => watershed => label image, remove small blobs
#                                     ||
#                                       dist_transform => threshold => label (fg)
# Distinctly label all components
def label_components(img):
    # label connected components in image.
    labeled_img, count = spimg.label(img)
    # obtain list of tuple, which are slices of array with distinct label
    slices = spimg.find_objects(labeled_img)

    return labeled_img, slices

def cell_size(labeled_img, slices):
    area_sum = 0
    cell_size_max = 0
    cell_index = {}
    for i in range(len(slices)):
        area = np.count_nonzero(labeled_img[slices[i]])
        area_sum += area
        cell_index[i] = [area]
        if area > cell_size_max:
            cell_size_max = area
    average_cell_size = area_sum / len(slices)

    return cell_index, average_cell_size, cell_size_max

# opencv's circle() is used to plot cells. The row and column number is fed to y and x coordinates for opencv.
def center_of_mass(labeled_img, cell_index, slices):
    for key in cell_index.keys():
        coordinates = tuple(np.around(spimg.center_of_mass(labeled_img[slices[key]])))
        x_delta = int(slices[key][0].start)
        y_delta = int(slices[key][1].start)
        x = int(coordinates[0]) + x_delta
        y = int(coordinates[1]) + y_delta
        # opencv format coordinate
        coordinates = (y, x)
        cell_index[key].append(coordinates)

    return cell_index

# filter small blobs. update remaining components labeling index
def filter_small_blobs(cell_index, size_threshold):
    for index in cell_index.keys():
        if cell_index[index][0] < size_threshold:
            del cell_index[index]

    return cell_index

# extract background by dilation. dilate with ring to augment cell body shape
def bg_extract(img):
    struct = np.ones((5, 5), dtype=np.uint8)
    struct[1:3, 1:3] = 0
    bg_img = cv2.dilate(img, struct)
    return bg_img


# foreground extraction by distance transform thresholding
def fg_extract(img):
    # Distance transform. Output 32-bit float image.
    dist_transform = cv2.distanceTransform(img, cv2.cv.CV_DIST_L1, maskSize=5)
    print("max of dist_transform is " + str(dist_transform.max()))
    non_zero_d = dist_transform[dist_transform != 0];
    mean_dist = np.mean(non_zero_d)
    print("mean of non_zero_d is " + str(mean_dist))
    ostuval, fg_img = cv2.threshold(dist_transform, mean_dist, 255, cv2.THRESH_BINARY)
    #struct = np.ones((5, 5), dtype=np.uint8)
    #struct[1:3, 1:3] = 0
    #fg_img = cv2.erode(img, struct)
    return fg_img, mean_dist

def watershed(gray_img, color_img):
    # extract fg and bg, subtract to obtain unknown
    fg_img, mean_dist_transform = fg_extract(gray_img)
    bg_img = bg_extract(gray_img)
    unknown = np.subtract(bg_img, fg_img)
    scipy.misc.imsave("fg_" + sys.argv[1], fg_img)
    scipy.misc.imsave("bg_" + sys.argv[1], bg_img)
    scipy.misc.imsave("unknown_" + sys.argv[1], unknown)
    # label fg. label bg as 1, unknown as 0.
    markers, placeholder = spimg.label(fg_img)
    markers += 1
    markers[unknown == 255] = 0
    markers = np.int32(markers)
    cv2.watershed(color_img, markers)
    gray_img[markers == -1] = 0

    return gray_img, mean_dist_transform

def draw_cell(img, cell_index):
    reconstruct = np.ndarray(img.shape, dtype=np.uint8)
    reconstruct.fill(255)
    for key in cell_index.keys():
        radius = np.int(np.sqrt(cell_index[key][0] / np.pi))
        cv2.circle(reconstruct, cell_index[key][1], radius, (255, 153, 0), thickness=-1)

    return reconstruct

def cell_count_watershed(gray, original):
    gray, mean_dist_transform = watershed(gray, original)
    scipy.misc.imsave("watershed_" + sys.argv[1], gray)
    # slices: list of tuples. Each tuple contains [xlow, xhigh) and [ylow, yhigh)
    labeled_img, slices = label_components(gray)
    # cell_index: a dictionary. key is cell index, value is a list, containing cell size, cell center of mass
    cell_index, average_cell_size, cell_size_max = cell_size(labeled_img, slices)
    print ("average component size: " + str(average_cell_size))
    size_threshold = 10.5 * mean_dist_transform
    # filter small components.
    cell_index = filter_small_blobs(cell_index, size_threshold)
    # obtain center of mass
    cell_index = center_of_mass(labeled_img, cell_index, slices)
    # reconstruct as solid circle
    reconstruct = draw_cell(original, cell_index)
    print("Cell counts in " + sys.argv[1] + ": " + str(len(cell_index)))
    scipy.misc.imsave("reconstruct_" + sys.argv[1], reconstruct)

def main():
    assert len(sys.argv) > 1, "image name not provided"
    assert len(sys.argv) < 3, "too many inputs"
    original = cv2.imread(sys.argv[1], cv2.IMREAD_COLOR)
    gray = cv2.imread(sys.argv[1], cv2.IMREAD_GRAYSCALE)
    cell_count_watershed(gray, original)

if __name__ == "__main__":
    main()