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
pushd "$SCRIPT_DIR" &> /dev/null

function compile()
{
    local ret_val=0

    pushd build/ &> /dev/null
    
    cmake -S ../ -B . -G Ninja && ninja clean && ninja all
    # cmake -S ../ && make clean && make all 
    ret_val=$?
    
    popd &> /dev/null # build/

    return $ret_val
}

function copy_utils()
{
    local ret_val=0

    if [ -d "bin/Debug/" ]; then
        cp utils/texler.sh bin/Debug/texler
        chmod +x bin/Debug/texler
    fi

    if [ -d "bin/Release/" ]; then
        cp utils/texler.sh bin/Release/texler
        chmod +x bin/Release/texler
    fi

    return $ret_val
}

mkdir -p build/ &> /dev/null

compile
copy_utils
    
popd &> /dev/null # $SCRIPT_DIR
