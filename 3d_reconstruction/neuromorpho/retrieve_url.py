# this script will retrieve html files for each cell type
# note: "/" in cell type name lead to failure to save file after retrieval. illegal filename
import os
from collections import deque
import urllib

prefix = "http://neuromorpho.org/getdataforbycell.jsp?class="
fail_log = open("fail_log.txt", "a")


def retrieve_from_deq(d, fail):
    while len(d) > 0:
        # if fail to retrieve 3 times, log the cell type and discard from deque
        if fail > 3:
            fail_log.write(d[0])
            print(d.popleft() + " failed to retrieve. \n")
        cell_type = d[0]
        # try retrieve html file for this cell type
        try:
            query = prefix + cell_type.replace(" ", "%20").rstrip("\n")
            # replace white space as _, / as \ to conform to filename convention
            urllib.urlretrieve(query, "/projects/3d_reconstruction/neuromorpho/html/" + cell_type.replace(" ", "_").rstrip("\n").replace("/", "\\") + ".html")
            fail = 0
            print(d.popleft() + ".html retrieved. \n")
        # if fail to retrieve, re-attempt until 3 tries
        except IOError:
            fail += 1
            retrieve_from_deq(d, fail)


def main():
    # create /html in cwd if not already created
    if not os.path.isdir(os.path.dirname(os.path.realpath(__file__)) + "/html"):
        os.mkdir(os.path.dirname(os.path.realpath(__file__)) + "/html")

    # create deque for cell_type.txt
    with open("/projects/3d_reconstruction/neuromorpho/cell_type_list.txt") as f:
        cell_type_deque = deque(f)
    # retrieve all cell_type.html files from neuromorpho.org
    retrieve_from_deq(cell_type_deque, 0)

if __name__ == "__main__":
    main()
