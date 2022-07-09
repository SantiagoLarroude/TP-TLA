#!/usr/bin/bash
#
# MIT License
# This file is part of texler
# 
# Copyright (c) 2022 Borracci, A.; Hinojo, N.; Larroudé Álvarez S.; Zahnd, M. E.
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
readonly SCRIPT_DIR="$(dirname ${BASH_SOURCE[0]})"

chmod +x "$SCRIPT_DIR/texler_compiler"

if [ ${#@} -eq 0 ]; then
    echo -e "\e[31mError\e[0m"
    echo "Please provide an input file."
    echo "Compilation terminated."
    exit 1
fi

for (( opt_i=1; opt_i <= ${#}; opt_i++)); do
   if [ ! -f "${!opt_i}" ]; then
       echo -e "\e[31mError\e[0m"
       echo "Argument number $opt_i is not a regular file:"
       echo "${!opt_i}"
       exit 1
   fi
done

function compile_file()
{
    local ret_val=0

    local texler_source="$1"
    local source_name="$(basename $1)"
    source_name="${source_name%.*}"
    local c_source="$(mktemp -u -p . ${source_name}_XXX.c)"

    "$SCRIPT_DIR"/texler_compiler < "$texler_source" > "$c_source"
    ret_val=$?

    if [ $ret_val -ne 0 ]; then
        return $ret_val
    fi

    gcc \
        --std=c11 \
        -Wall \
        -g \
        "$c_source" \
        -o "$source_name.elf"
    ret_val=$?
    
    return $ret_val
}

max_return=0

for (( opt_i=1; opt_i <= ${#}; opt_i++)); do
    ret_val=0
    compile_file "${!opt_i}"
    ret_val=$?

    max_return=$(( $ret_val > $max_return ? $ret_val : $max_return ))
done

exit $max_return
