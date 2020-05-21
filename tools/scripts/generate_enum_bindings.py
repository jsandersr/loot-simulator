import generate_cpp_util as cpp_util

import glob
import json
import logging
import os
import re
import sys
import string

HEADER_FILE_NAME = 'EnumDataBindings.h'

GENERATED_DIR_NAME = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                  '../../loot-simulator/generated')
ENUM_DATA_BINDINGS_FILE_PATH = os.path.join(GENERATED_DIR_NAME,
                                            'EnumDataBindings.h')
RESOURCES_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                              '../../resources')
MONSTER_DATA_PATH = os.path.join(RESOURCES_PATH, 'monsters.json')
TABLE_DATA_PATH = os.path.join(RESOURCES_PATH, 'loot-tables')

MONSTER_TYPE_NAME = 'MonsterType'
ITEM_TYPE_NAME = 'TreasureType'

# These lists will be generated from the json data.
MONSTER_IDS_TO_ENUM = {'none': 'NONE'}
ITEM_IDS_TO_ENUM = {'none': 'NONE'}


def id_to_enum(id):
    enum_id = ""
    for char in id:
        if char.isupper():
            enum_id += '_'
        enum_id += char.upper()
    return enum_id


def init_monster_enums():
    with open(MONSTER_DATA_PATH) as json_file:
        data = json.load(json_file)

    for m in data['monsters']:
        type_id = m['type']
        MONSTER_IDS_TO_ENUM[type_id] = ""

    for id in MONSTER_IDS_TO_ENUM:
        MONSTER_IDS_TO_ENUM[id] = id_to_enum(id)


def init_item_enums():
    validPaths = TABLE_DATA_PATH + '/*.json'
    for filepath in glob.iglob(validPaths):
        with open(filepath) as json_file:
            data = json.load(json_file)

            for m in data['items']:
                type_id = m['type']
                ITEM_IDS_TO_ENUM[type_id] = ""

            for id in ITEM_IDS_TO_ENUM:
                ITEM_IDS_TO_ENUM[id] = id_to_enum(id)


def write_code_to_header(code_string):
    try:
        with open(ENUM_DATA_BINDINGS_FILE_PATH,
                  'w+', encoding='utf-8') as enum_bindings_header:
            enum_bindings_header.write(code_string)
    except OSError as e:
        logging.error('Failed to create html dump. ' +
                      ' error=' + e.strerror +
                      ' file_name=' + path)


def clear_file(file_name):
    if os.path.isfile(file_name):
        file = open(file_name, 'w')
        file.close()


def clear_files(files):
    for file in files:
        clear_file(file)


def initialize_dirs():
    os.makedirs(GENERATED_DIR_NAME, exist_ok=True)


def enumerate_ids_as_enum(ids):
    enum_value_string = ''
    for i, (k, v) in enumerate(ids.items()):
        enum_value_string += cpp_util.get_indentation_spaces(1)
        enum_value_string += v
        if i == 0:
            enum_value_string += ' = -1'
        elif i == 1:
            enum_value_string += ' = 0'
        enum_value_string += ',\n'
    enum_value_string += cpp_util.get_indentation_spaces(1)
    enum_value_string += 'NUM_TYPES\n'
    return enum_value_string


def get_enum_string(ids, typeName):
    return (cpp_util.get_enum_signature('int32_t', typeName) +
            cpp_util.get_new_line() +
            cpp_util.get_scope_open() +
            cpp_util.get_new_line() +
            enumerate_ids_as_enum(ids) +
            cpp_util.get_adt_scope_close())


def get_monster_enum_string():
    return (cpp_util.get_enum_signature('int32_t', 'MonsterType') +
            cpp_util.get_new_line() +
            cpp_util.get_scope_open() +
            cpp_util.get_new_line() +
            enumerate_ids_as_enum(MONSTER_IDS_TO_ENUM) +
            cpp_util.get_adt_scope_close())


def get_enum_bindings(ids, typeName):
    binding_string = ''
    for i, (k, v) in enumerate(ids.items()):
        binding_string += (
            cpp_util.get_indentation_spaces(2) + 
            '{ ' + typeName + '::' + v + ', ' + "\"" + k + "\" },\n")

    return binding_string


def get_binding_macro(ids, typeName):
    return ('NLOHMANN_JSON_SERIALIZE_ENUM(' +
            typeName +
            ',\n' +
            cpp_util.get_indentation_spaces(1) + 
            '{\n' +
            get_enum_bindings(ids, typeName) +
            cpp_util.get_indentation_spaces(1) + 
            '});\n')


def build_code_string():
    return (cpp_util.get_file_info_comment(HEADER_FILE_NAME) +
            cpp_util.get_include_guard() +
            cpp_util.get_generated_file_warning() +
            cpp_util.get_include_system_string('nlohmann/json/json.hpp') +
            cpp_util.get_new_line() +
            cpp_util.get_namespace_opener('LootSimulator') +
            get_enum_string(MONSTER_IDS_TO_ENUM, MONSTER_TYPE_NAME) +
            cpp_util.get_new_line() +
            get_enum_string(ITEM_IDS_TO_ENUM, ITEM_TYPE_NAME) +
            cpp_util.get_new_line() +

            get_binding_macro(MONSTER_IDS_TO_ENUM, MONSTER_TYPE_NAME) +
            cpp_util.get_new_line() +
            get_binding_macro(ITEM_IDS_TO_ENUM, ITEM_TYPE_NAME) +
            cpp_util.get_new_line() +
            cpp_util.get_namespace_closer('LootSimulator'))


def main():
    initialize_dirs()
    init_monster_enums()
    init_item_enums()

    files_to_clear = [ENUM_DATA_BINDINGS_FILE_PATH]
    clear_files(files_to_clear)

    code_string = build_code_string()
    write_code_to_header(code_string)


if __name__ == '__main__':
    main()
