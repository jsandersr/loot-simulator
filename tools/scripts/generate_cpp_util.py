# -*- coding: utf-8 -*-
""" generate_cpp_util.py

This module is intended to contain utility functions for generating C++ code.
Some utility methods return a string with specific formatting while others are
meant to be more flexible in their composability. This is simply for
convenience.

Example:
    `get_namespace_closer(Foo)` will return `} // namespace Foo`

In this case, get_namespace_closer() uses get_scope_close() to compose its
string. The number of new lines each function returns is deliberate and
intentional.

Some functions such as get_scope_open() return a base component intended to
be more flexible in their formatting, i.e they do not include any new lines,
spaces, or other bits of formatting.

In cases where there is no situation in which a new line wouldn't be added
manually, new lines are added appended to the end. This is usually done only in
convenience functions with special formatting.

"""

import string

SPACES_PER_TAB = 4


def get_semicolon() -> string:
    """No special formatting included, intended to be a simple component.

    Example:
        `;`

    Returns:
        A semicolon.

    """
    return ';'


def get_new_line() -> string:
    """No special formatting included, intended to be a simple component.

    Returns:
        A new line.

    """
    return '\n'


def get_single_line_comment() -> string:
    """No special formatting included, intended to be a simple component.

    Example:
        `//`

    Returns:
        A comment opener for a single line comment.

    """
    return '//'


def get_line_comment_normal() -> string:
    """No special formatting included, intended to be a simple component.

    Example:
        `//----------------------------------------------------------------`

    Returns:
        A single dotted line used as formatters in special cases, such as at
        the top of a file.

    """
    return ('//----------------------------------------------------------'
            '---------------------')


def get_line_comment_double() -> string:
    """No special formatting included, intended to be a simple component.

    Example:
        `//================================================================`

    Returns:
        A single, double-dotted line used as formatters in special cases,
        such as at the top of a file.

    """
    return ('//=========================================================='
            '=====================')


def get_scope_open(indentation_level_count=0) -> string:
    """ Convenience function for open scope operator.

    The open scope returned is formatted according to the indentation level
    specified. No new lines are included at the end.

    Example:
        ```
        {
        or one with 4 spaces before
            {
        ```
    Args:
        indentation_level_count(int): Indicates how nested the scope is. This
        argument is simply forwarded along.

    Returns:
        An open scope operator '{'. No new lines included intentionally.
        This decision was purely made to be consistent with get_scope_close().

    """
    return get_indentation_spaces(indentation_level_count) + '{'


def get_scope_close(indentation_level_count=0) -> string:
    """ Convenience function for open scope operator.

    The open scope returned is formatted according to the indentation level
    specified. No new lines are included at the end.

    Example:
        ```
        }
        or one with 4 spaces before
            }
        ```

    Args:
        indentation_level_count(int): Indicates how nested the scope is. This
        argument is simply forwarded along.

    Returns:
        An open scope operator '{'. No new lines included intentionally.
        This decision was purely made to be consistent with get_scope_close().

    """
    return get_indentation_spaces(indentation_level_count) + '}'


def get_indentation_spaces(indentation_level_count=0) -> string:
    """ Convenience function for composable indentation.

    This function is intended to be used when composing lines of code
    that require indentation. No new lines are added to the resulting string.

    Args:
        indentation_level_count(int): If nothing is passed, and 0 is used, any
        code string composed with this as its indentation scheme will be
        written on the far left of the file. Spaces are accumulated by
        count * tab size.

    Returns:
        A string of spaces according to the indentation level.

    """

    spaces = ''
    for i in range(indentation_level_count):
        for space in range(SPACES_PER_TAB):
            spaces += ' '
    return spaces


def get_file_info_comment(file_name) -> string:
    """ Convenience function that provides file info comment.

    All cpp and header files must have a file info comment at the top of
    the file according to the Hydra style guide.

    Example:
        ```
        //-----------------------------------------------
        //
        // MyFile.h
        //
        ```

    Args:
        file_name(string): Required. Specifies the name of the file that
        will appear in the file info comment.

    Returns:
        A string containing the entire file info comment. Two new lines
        are added at the end as there is no case in which this wouldn't be
        added manually to the file_info_comment.
    """

    return (get_line_comment_normal() +
            get_new_line() +
            get_single_line_comment() +
            get_new_line() +
            get_file_name_comment(file_name) +
            get_single_line_comment() +
            get_new_line() + get_new_line())


def get_include_system_string(include_name) -> string:
    """ Used to generate an include string for a system library.

    This should be used for system level includes such as vector,
    functional, memory, etc. A new line is appened to the end as there is
    no scenario where this wouldn't be added manually.

    Example:
        `#include <memory>`

    Args:
        include_name(string): Required. Specifies the name of the include
        that will be written in the include statement.

    Returns:
        An include statement with `<>`

    """
    return '#include <' + include_name + '>\n'


def get_namespace_opener(namespace_name) -> string:
    """ Convenience function used to generate a namespace opening statement.

    The namespace statement has the open scope on the same line. This is
    intended to use for top-level namespaces that would be declared towards
    the beginning of the file. These namespace declarations are always
    followed by a double dotted line.

    Example:
        ```
        namespace MyNamespace {

        //=====================================================
        ```

    Args:
        namespace_name(string): Required. Specifies the name of the namespace
        that the namespace opener will be written for.

    Returns:
        A namespace opener according to the Hydra C++ style guide. New
        lines are appended to the end since there is no case where they would
        not be added manually.

    """
    return ('namespace ' + namespace_name + ' ' + get_scope_open() +
            '\n\n' + get_line_comment_double() +
            '\n\n')


def get_namespace_closer(namespace_name) -> string:
    """ Convenience function used to generate a namespace closing statement.

    Closes a namespace according to the Hydra C++ style guide.This is
    intended to use for top-level namespaces that would be declared towards
    the end of the file. These namespace declarations are always preceded
    by a double dotted line, and have a commend after the closing scope
    operator.

    Example:
        ```
        //=================================================================

        } // namespace MyNamespace
        ```

    Args:
        namespace_name(string): Required. Specifies the name of the namespace
        that the namespace closer will be written for.

    Returns:
        A namespace closer.

    """
    return (get_line_comment_double() + '\n\n' + get_scope_close() +
            ' // namespace ' + namespace_name + '\n')


def get_include_guard() -> string:
    """ Convenience function used to generate an include guard.

    Uses "pragma once" as the include guard. New lines are added as there is
    no case where they would not be added manually.

    Example:
        `#pragma once`

    Returns:
        A string containing the pragma once include guard.

    """
    return '#pragma once\n\n'


def get_generated_file_warning() -> string:
    """ Returns a comment containing a warning string.

    Its important to be clear that the file generated by this script is in fact
    generated. Therefore, any changes to the file made directly will be erased
    upon regenerating the script.

    Returns:
        A string containing a comment with a warning about the file being
        generated.

    """
    return '// This is a generated file! Any changes here will be lost!!!\n\n'


def get_file_name_comment(file_name) -> string:
    """ Convenience function for getting a comment starter and file name.

    According to the Hydra C++ style guide, all file info comments must contain
    a comment with the file name. This function will return that, given the
    file name.

    A new line is appended as there is no case in which it would not be
    appended manually.

    Example:
        `// MyFile.h`

    Args:
        file_name(string): the name to be included in the comment string.

    Returns:
        A file comment.

    """
    return get_single_line_comment() + ' ' + file_name + '\n'


def get_adt_scope_close() -> string:
    """ Convenience function for closing the scope of an ADT

    User defined types such as those defined with classes, structs, enums, etc
    must have their scopes closed with a semicolon. This function provides
    that.

    A new line is appended as there is no case in which it would not be
    appended manually.

    Example:
        `};`

    Returns:
        A scope closure with a `;`.

    """
    return get_scope_close() + get_semicolon() + '\n'


def get_enum_signature(underlying_type, enum_name, is_class=False) -> string:
    """ Forms the signature of an enum

    In order to define an enum, an enum signature must be provided. Using this
    function we can provide an enum class or struct, with any specified
    underlying type.

    Scoping is not provided and should be composed by the user.

    Example:
        `enum class GameIds : uint32_t`

    Args:
        underlying_type(string): Specifies the type. ex. uint32_t, int, etc.
        enum_name(string): The name of the enum.
        is_class(bool): Setting this flag to true will cause an enum class to
        be created rather than an enum struct.

    Returns:
        An enum signature.

    """
    type_string = 'struct'
    if is_class is True:
        type_string = 'class'

    return ('enum ' + type_string + ' ' + enum_name + ' : ' + underlying_type)
