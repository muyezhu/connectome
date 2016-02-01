# this script will parse cell type, author, species, and individual neuron
# information from cell_type.html file
import os
from bs4 import BeautifulSoup
import retrieve_neuron


def archived(cell_type):
    if not os.path.isfile(os.path.dirname(os.path.realpath(__file__)) + "/archive_html.txt"):
        return False
    with open(os.path.dirname(os.path.realpath(__file__)) + "/archive_html.txt", "r") as f:
        for line in f:
            if cell_type == line.rstrip():
                return True
        return False


# traverse all html files in neuromorpho/html/
def main():
    for html in os.listdir(os.path.dirname(os.path.realpath(__file__)) + "/html"):
        cell_type = html.replace(".html", "")
        # if the cell_type is archived, do not parse the html file
        if archived(cell_type):
            print(cell_type + " complete" + "\n")
            continue
        with open(os.path.dirname(os.path.realpath(__file__)) + "/html/" + html, "r") as f:
            # create soup
            soup = BeautifulSoup(f, "html.parser")
            # parse author, species, region, neuron name
            # author name contained in <font> tag with id = lvl2
            for author_tag in soup.find_all(id="lvl2"):
                # species <font> tag <input> subtag has id equal to value of author <font> tag <input> subtag
                # same rule to find region_tag from species tag, and neuron_tag from region_tag
                for species_tag in author_tag.find_all_next(id=author_tag.input["value"] + postfix):
                    species_tag = species_tag.parent
                    for region_tag in species_tag.find_all_next(id=species_tag.input["value"] + postfix):
                        # build path for each /cell_type/author/species/region/
                        region_tag = region_tag.parent
                        parent_path = os.path.dirname(os.path.realpath(__file__)) + "/data/" + cell_type + "/" + author_tag.contents[-1].string.replace(",", "_") + "/" + species_tag.contents[-1].string + "/" + region_tag.contents[-1].string + "/"
                        if not os.path.isdir(parent_path):
                            os.makedirs(parent_path)
                        # skip completed folder
                        if os.path.isfile(parent_path + "complete.html"):
                            continue
                        # parse neuron names
                        # the following is a search term for cells in same author, species and region tree
                        search = region_tag.input['value'] + postfix
                        no_fail = True
                        # retrieve information for each neuron
                        for item in soup.find_all(id=search):
                            # typical output for item:
                            # <input id="TmY-type unknown-1ChklovskiiDrosophila melanogasterRight Optic Lobe_chkbox" name="neuron_name" onclick="fnClickCheckbox(this)"
                            # type="checkbox" value="280277"/>
                            neuron_tag = item.next_sibling.next_sibling
                            if not retrieve_neuron.retrieve_neuron(neuron_tag, parent_path):
                                no_fail = False
                        if no_fail:
                            f = open(parent_path + "complete.html", "w")
                            f.close()
        # archive html files where all retrieval are completed
        with open(os.path.dirname(os.path.realpath(__file__)) + "/archive_html.txt", "a") as f:
            f.write(cell_type + "\n")
        print(cell_type + " complete" + "\n")
    print("All complete")

if __name__ == "__main__":
    prefix = "http://neuromorpho.org/"
    postfix = "_chkbox"
    main()
