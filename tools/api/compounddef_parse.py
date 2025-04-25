# Copyright 2025 The Lynx Authors. All rights reserved.
# Licensed under the Apache License Version 2.0 that can be found in the
# LICENSE file in the root directory of this source tree.

# /usr/bin/env python3
# -*- coding: utf-8 -*-

from memberdef_parse import (
    function_parse,
    variable_parse,
    property_parse,
    typedef_parse,
    enum_parse as memberdef_enum_parse,
    define_parse,
)


def compounddef_kind_check(compounddef, kind: str) -> bool:
    if compounddef == None or compounddef.get("kind") != kind:
        print(
            f'compounddef function parse failed, wrong kind: {compounddef.get("kind")} expected kind: {kind}'
        )
        return False
    return True


MEMBERDEF_PARSE_DICT = {
    "function": function_parse,
    "variable": variable_parse,
    "property": property_parse,
    "typedef": typedef_parse,
    "enum": memberdef_enum_parse,
}

MEMBERDEF_IGNORE_LIST = ["define"]


def memberdef_parse(compounddef, header: str, end: str) -> list[str]:
    member_list = []
    for memberdef in compounddef.findall(".//memberdef"):
        if memberdef.get("prot") != "public":
            continue
        memberdef_kind = memberdef.get("kind")
        memberdef_parse_func = MEMBERDEF_PARSE_DICT.get(memberdef_kind)
        if memberdef_parse_func is not None:
            member_list.append(memberdef_parse_func(memberdef))
        elif memberdef_kind not in MEMBERDEF_IGNORE_LIST:
            print(f"unknown memberdef: {memberdef_kind}")

    if not member_list:
        return []
    if header:
        member_list = [header] + member_list
    if end:
        member_list.append(end)
    return member_list


def class_parse(compounddef) -> list[str]:
    if not compounddef_kind_check(compounddef, "class"):
        return ""

    class_name = compounddef.find("compoundname").text
    abstract_str = "abstract " if compounddef.get("abstract") == "yes" else ""
    class_str = f"public class {abstract_str}{class_name} : "
    base_class_list = []
    for basecompoundref in compounddef.findall(".//basecompoundref"):
        base_class_list.append(basecompoundref.text)
    class_str += ", ".join(base_class_list)
    class_str += " {"

    return memberdef_parse(compounddef, class_str, "}")


def enum_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "enum"):
        return ""
    enum_header = f'public enum {compdef.find("compoundname").text} {{'
    return memberdef_parse(compdef, enum_header, "}")


def interface_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "interface"):
        return ""
    interface_header = f'public interface {compdef.find("compoundname").text} {{'
    return memberdef_parse(compdef, interface_header, "}")


def file_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "file"):
        return ""
    return memberdef_parse(compdef, "", "")


def struct_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "struct"):
        return ""
    struct_header = f'public struct {compdef.find("compoundname").text} {{'
    return memberdef_parse(compdef, struct_header, "}")


def category_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "category"):
        return ""
    category_header = f'public interface {compdef.find("compoundname").text} {{'
    return memberdef_parse(compdef, category_header, "}")


def protocol_parse(compdef) -> list[str]:
    if not compounddef_kind_check(compdef, "protocol"):
        return ""
    base_class_list = []
    for basecompoundref in compdef.findall(".//basecompoundref"):
        base_class_list.append(basecompoundref.text)
    protocol_header = f'public protocol {compdef.find("compoundname").text} : {", ".join(base_class_list)} {{'
    return memberdef_parse(compdef, protocol_header, "}")
