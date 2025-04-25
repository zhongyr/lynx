# Copyright 2025 The Lynx Authors. All rights reserved.
# Licensed under the Apache License Version 2.0 that can be found in the
# LICENSE file in the root directory of this source tree.

# /usr/bin/env python3
# -*- coding: utf-8 -*-


def kind_check(memberdef, kind: str) -> bool:
    if memberdef == None or memberdef.get("kind") != kind:
        print(
            f'memberdef function parse failed, wrong kind: {memberdef.get("kind")} expected kind: {kind}'
        )
        return False
    return True


def function_parse(memberdef) -> str:
    if not kind_check(memberdef, "function"):
        return ""
    definition = memberdef.find("definition").text
    args = memberdef.find("argsstring").text
    return f"  public {definition}{args};"


def variable_parse(memberdef) -> str:
    if not kind_check(memberdef, "variable"):
        return ""
    definition = memberdef.find("definition").text
    name = memberdef.find("name").text
    return f"  public {definition} {name};"


def property_parse(memberdef) -> str:
    if not kind_check(memberdef, "property"):
        return ""
    definition = memberdef.find("definition").text
    name = memberdef.find("name").text
    return f"  public {definition} {name};"


def typedef_parse(memberdef) -> str:
    if not kind_check(memberdef, "typedef"):
        return ""
    return f'  {memberdef.find("definition").text}'


def enum_parse(memberdef) -> str:
    if not kind_check(memberdef, "enum"):
        return ""
    enum_str = f'  public enum {memberdef.find("name").text}'
    for enumvalue in memberdef.findall("enumvalue"):
        enum_str += f'\n  {enumvalue.find("name").text}'
    return enum_str


def define_parse(memberdef) -> str:
    if not kind_check(memberdef, "define"):
        return ""
    define_str = f'  define {memberdef.find("name").text}'
    defname_list = [defname.text() for defname in memberdef.findall("defname")]
    define_str += " ,".join(defname_list) + "\n"
    initializer_element = memberdef.find("initializer")
    if initializer_element:
        define_str += initializer_element.text + "\n"
    return define_str
