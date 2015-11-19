import scipy.misc
import numpy as np
import image_component

def label_connected_component(img):
    # dictionary key: label. value: [size, [i, j]lists]
    connected_component = {}
    a, b = img.shape
    label = 0
    for i in xrange(a):
        img, connected_component, label = label_neighbor(i, img, label, b, connected_component)
    return img, connected_component, len(connected_component)

##@profile
def label_neighbor(i, img, label, b, connected_component):
    row_neighborhood_size = 0
    for j in xrange(b):
        # if pixel is background, skip loop
        if not img[i][j]:
            row_neighborhood_size = 0
            continue
        row_neighborhood_size += 1
        # if new row wise component, label += 1
        # local_label: the current label index, starts same as label, can be updated during component joining.
        # label: equals to the index used when the current row neighborhood's labeling initialized. ensure non overlap.
        if row_neighborhood_size == 1:
            label += 1
            local_label = label
        # pixel is always first labeled by right wards movement in the same row.
        img[i][j] = local_label
        # if label is previously collected, append array index, increment size
        if local_label in connected_component:
            connected_component[local_label].add_pixel(i, j)
            connected_component[local_label].size += 1
        # else, initialize entry
        else:
            component = image_component.ImageComponent(local_label, i, j)
            connected_component[local_label] = component

        # then search 1 position up, except for first row
        if not i:
            continue
        # if top pixel is labeled, merge, delete component with smaller size
        if img[i - 1, j]:
            # if top pixel and current pixel have same label, no operation done.
            if img[i - 1, j] == img[i,j]:
                continue
            top_pixel_label = img[i - 1, j]
            # change current labeling if current label component is smaller. label row neighbor with top_pixel_label
            if connected_component[top_pixel_label].size >= connected_component[local_label].size:
                connected_component[top_pixel_label].join(connected_component[local_label])
                img, connected_component = re_label_neighbor(img, connected_component, local_label, top_pixel_label)
                local_label = top_pixel_label
            # change top labeling if top labeling component is smaller
            else:
                connected_component[local_label].join(connected_component[top_pixel_label])
                img, connected_component = re_label_neighbor(img, connected_component, top_pixel_label, local_label)
    return img, connected_component, label

def re_label_neighbor(img, connected_component, old_index, new_index):
    connected_componnet_coordinates = connected_component[old_index].coordinates
    for coordinates in connected_componnet_coordinates:
        img[coordinates[0], coordinates[1]] = new_index
    del connected_component[old_index]
    return img, connected_component


def get_test_array(size):
    import random
    test_image = np.zeros((size, size), dtype = np.uint32)
    for i in range(test_image.shape[0]):
        for j in range(test_image.shape[1]):
            test_image[i][j] += random.randint(0,1)

    return test_image


# Note: imread default flatten = 0, where img is read as rgb.

img = scipy.misc.imread('ctb_lat.tif', 1)
img, connected_component, count = label_connected_component(img)
print count



