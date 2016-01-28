# test concept of "current working directory"
# in bash, cd to /muye
# from /muye, call /muye/projects/3d_reconstruction/neuromorpho/test.py
# within test.py, call function os.getcwd()

import os
print os.getcwd()
# result: /ifs/loni/faculty/dong/mcp/muye
print os.path.realpath(__file__)
# result: /ifs/loni/faculty/dong/mcp/muye/projects/3d_reconstruction/neuromorpho/test.py