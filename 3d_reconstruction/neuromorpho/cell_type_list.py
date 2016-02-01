import urllib
from bs4 import BeautifulSoup


def main():
    # save html page to file
    urllib.urlretrieve("http://neuromorpho.org/bycell.jsp", "/projects/3d_reconstruction/neuromorpho/by_celltype.html")
    # make soup
    soup = BeautifulSoup(open("/projects/3d_reconstruction/neuromorpho/by_celltype.html"), "html.parser")
    # with BeautifulSoup parse list of link of cell type texts
    # manually remove prevailing and trailing iirelevant texts
    f = open("/projects/3d_reconstruction/neuromorpho/cell_type_list.txt", "w")
    for string in soup.stripped_strings:
        # encode() call is required. default codecs is ascii
        f.write(string.encode("UTF-8") + "\n")

if __name__ == "__main__":
    main()
