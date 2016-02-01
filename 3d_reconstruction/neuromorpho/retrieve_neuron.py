# this script will retrieve information, .swc and img for individual neuron
import os
import urllib
from bs4 import BeautifulSoup

prefix = "http://neuromorpho.org/"


# neuron is a tag, typically in format as:
# # <b><a class="screenshot" href="neuron_info.jsp?neuron_name=280277"
# rel="./images/imageFiles/Chklovskii/280277.png" rev="280277"
# target="_blank"> 280277</a></b>
def retrieve_neuron(neuron, parent_path):
    neuron_status = collected(neuron, parent_path)
    if neuron_status == 3:
        return
    neuron_url = prefix + neuron.a["href"]
    soup = make_soup(neuron_url)
    if soup is not None:
        if neuron_status < 1:
            retrieve_info(soup, neuron, parent_path)
        if neuron_status < 2:
            retrieve_swc(soup, neuron, parent_path)
        if neuron_status < 3:
            retrieve_img(soup, neuron, parent_path)


# do not retrieve existing files
def collected(neuron, parent_path):
    if not os.path.isfile(parent_path + neuron.string +".txt"):
        return 0
    if not os.path.isfile(parent_path + neuron.string +".swc"):
        return 1
    if not os.path.isfile(parent_path + neuron.string +".png"):
        return 2
    return 3


# soup for html page @ prefix + neuron["href"]
def make_soup(neuron_url, fail=0):
    if fail == 3:
        with open("neuron_fail_log.txt", "a") as neuron_fail_log:
            neuron_fail_log.write(neuron_url + "\n")
        print("fail to open url " + neuron_url + "\n")
        return None
    try:
        soup = BeautifulSoup(urllib.urlopen(neuron_url))
        return soup
    except IOError:
        fail += 1
        make_soup(neuron_url, fail=fail)


def retrieve_info(soup, neuron, parent_path):
    with open(parent_path + "/" + neuron.string + ".txt", "w") as info:
        for key in soup.find_all(align="right"):
            if key.string is None:
                continue
            info.write(key.string.encode("utf-8") + "  ")
            value = key.find_next("td")
            if value.string is None:
                info.write("N/A\n")
            else:
                info.write(value.string.encode("utf-8").lstrip().rstrip() + "\n")


# Note: find("a", string=) function did not work for unknown reason
def retrieve_swc(soup, neuron, parent_path):
    # find standardized swc file link
    for a in soup.find_all("a"):
        if a.string is None:
            continue
        if a.string.find("Standard") >= 0:
            swc_link = prefix + a["href"]
            break

    def retrieve_swc_(fail=0):
        if fail == 3:
            with open("swc_fail_log.txt", "a") as swc_fail_log:
                swc_fail_log.write(parent_path + "\t")
                swc_fail_log.write(neuron.string.encode("utf-8") + "\n")
        try:
            urllib.urlretrieve(swc_link, parent_path + neuron.string + ".swc")
        except IOError:
            fail += 1
            retrieve_swc_(fail=fail)

    retrieve_swc_()


def retrieve_img(soup, neuron, parent_path):
    img = soup.find("img", ismap="ismap")
    img_link = prefix + img["src"].lstrip("./")

    def retrieve_img_(fail=0):
        if fail == 3:
            with open("img_fail_log.txt", "a") as img_fail_log:
                img_fail_log.write(parent_path + "\t")
                img_fail_log.write(img_link + "\n")
        try:
            urllib.urlretrieve(img_link, parent_path + neuron.string + ".png")
        except IOError:
            fail += 1
            retrieve_img_(fail=fail)

    retrieve_img_()
