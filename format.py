import os
import re
import subprocess

project_dir = os.path.dirname(os.path.abspath(__file__))

skip_patterns = [
    'stb_image.h'
]

def need_format(file):
    # choose files
    _, file_ext = os.path.splitext(file)
    if not (file_ext in ['.hpp', '.cpp', '.h', '.c', '.glsl', '.vert', '.frag']):
        return False

    # white list
    file_name_with_ext = os.path.basename(file)
    for pattern in skip_patterns:
        if re.search(pattern, file_name_with_ext):
            return False
    return True

def format_folder(folder_name):
    for root, _, files in os.walk(folder_name, topdown=False):
        for name in files:
            file_path = os.path.join(root, name)
            if need_format(file_path):
                print(f'*** formatting file {file_path}')
                subprocess.run(['clang-format', '-i', file_path])

for dir in ['source', 'resources/glsl']:
    format_folder(os.path.join(project_dir, dir))
