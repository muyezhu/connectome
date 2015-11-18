import scipy.ndimage
import scipy.misc
import image_component
import copy

# same functionality as connected_component, except that the indices are in 1 step, 1, 2, 3, 4, etc.
#@profile
def label_connected_component(img):
    # connected_component key: label. value: ImageComponent object
    connected_component = {}
    deleted_index = []
    a, b = img.shape
    global_label = 0
    for i in range(a):
        img, connected_component, global_label, deleted_index = label_neighbor(i, b, img, connected_component, global_label, deleted_index)
    return img, connected_component, len(connected_component)


def fetch_index(global_label, deleted_index):
    if deleted_index:
        local_label = deleted_index[-1]
        deleted_index.pop()
        return global_label, local_label
    global_label += 1
    return global_label, global_label

#@profile
def label_neighbor(i, b, img, connected_component, global_label, deleted_index):
    row_neighborhood_size = 0
    for j in xrange(b):
        # if pixel is foreground, label it.
        if img[i][j]:
            row_neighborhood_size += 1
            # local_label: the current label index, starts same as global_label, or top pixel label,
            # can be updated during component joining.
            # global_label: tracker to avoid duplicate already used label index
            if row_neighborhood_size == 1:
                if i > 0:
                    if img[i - 1][j]:
                        local_label = img[i - 1][j]
                    else:
                        global_label, local_label = fetch_index(global_label, deleted_index)
                else:
                    global_label, local_label = fetch_index(global_label, deleted_index)

            # pixel is labeled with local_label.
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

            # if top pixel is labeled, merge, delete component with smaller size
            if not i:
                continue
            if img[i - 1, j]:
                # if top pixel and current pixel have same label, no operation done.
                if img[i - 1, j] == img[i,j]:
                    continue
                top_pixel_label = img[i - 1, j]
                # change current labeling if current label component is smaller. label row neighbor with top_pixel_label
                if connected_component[top_pixel_label].size >= connected_component[local_label].size:
                    connected_component[top_pixel_label].join(connected_component[local_label])
                    img, connected_component = re_label_neighbor(img, connected_component, local_label, top_pixel_label)
                    deleted_index.append(local_label)
                    local_label = top_pixel_label
                # change top labeling if top labeling component is smaller
                else:
                    connected_component[local_label].join(connected_component[top_pixel_label])
                    img, connected_component = re_label_neighbor(img, connected_component, top_pixel_label, local_label)
                    deleted_index.append(top_pixel_label)
        else:
            row_neighborhood_size = 0
    return img, connected_component, global_label, deleted_index

def re_label_neighbor(img, connected_component, old_index, new_index):
    connected_componnet_coordinates = connected_component[old_index].coordinates
    for coordinates in connected_componnet_coordinates:
        img[coordinates[0], coordinates[1]] = new_index
    del connected_component[old_index]
    return img, connected_component

def get_test_array(size):
    import random
    import numpy as np
    test_image = np.zeros((size, size), dtype = np.uint32)
    for i in range(test_image.shape[0]):
        for j in range(test_image.shape[1]):
            test_image[i][j] += random.randint(0,1)

    return test_image


# Note: imread default flatten = 0, where img is read as rgb. dtype = float32
# Alert: When read with cv2.imread grayscale, array elements are uchar, integer overflow when components are
# numerous. Need to convert datatype
img = scipy.misc.imread('ctb_lat.tif', 1)
#img = get_test_array(10)

img2 = copy.deepcopy(img)
label_image, connected_component, count = label_connected_component(img)

correct_label, num = scipy.ndimage.label(img2)
print count
print num



