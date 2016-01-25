# this script will download .swc file and gather information table for each neuron
import os
import urllib
from bs4 import BeautifulSoup

# traverse all html files in neuromorpho/html/
for html in os.listdir(os.getcwd() + "/html"):
    with open(html, "r") as f:
        # create soup
        soup = BeautifulSoup(f, "html.parser")
        # parse author, species, region, neuron name
        # author name contained in <font> tag with id = lvl2
        for author_tag in soup.find_all(id="lvl2"):
            # author, species, region and form of neurons are siblings separated by <br>, '\n'
            species_tag = author_tag.next_sibling.next_sibling.next_sibling
            region_tag = species_tag.next_sibling.next_sibling.next_sibling
            # build path for each neuron: cwd/cell cype/author/species/region/
            parent_path = os.getcwd() + "/" + cell_type.replace(" ", "_") + "/" + author_tag.contents[-1].string.replace(",", "_") + "/" + species_tag.contents[-1].string + "/" + region_tag.contents[-1].string + "/"
            print(author_tag.contents[-1].string.replace(",", "_") + "\n" + "\t" + species_tag.contents[-1].string + "\n" + "\t\t" + region_tag.contents[-1].string)
            # parse neuron names
            # the following is a search term for cells in same author, species and region tree
            search = region_tag.input['value'] + "_chkbox"
            for item in soup.find_all(id = search):
                # make dir for each neuron
                neuron_tag = item.next_sibling.next_sibling
                os.makedirs(parent_path + neuron_tag.string)
                #print (item.next_sibling.next_sibling.string + "\n")

    break




# http://neuromorpho.org/getdataforbycell.jsp?class=Amacrine%20cell