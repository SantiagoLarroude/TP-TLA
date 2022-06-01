#!/usr/bin/bash

readonly compiler_path="../bin/Debug/texler"

# ['filename']=expected_return
declare -A test_files=(\
    ["r30"]=0 \
    ["r31"]=0 \
    ["r32"]=0 \
    ["r33"]=0 \
    ["r34"]=0 \
    ["r35"]=0 \
    ["r36"]=0 \
    ["r37"]=0 \
    ["r38"]=0 \
    ["r39"]=0 \
    ["r310"]=0 \
    ["r311"]=1 \
    ["r312"]=1 \
    ["r313"]=1 \
    ["r314"]=1 \
    ["r315"]=1 \
)

readonly test_logs="logs"

# Print colored string with date-time.
#
# Arguments:
#   $1 : Color number 
#        For red, you'll normally type \e[31m, then $1 should be '31'.
#   $* : String to print
#
# Returns:
#   0 : Always
function infoc()
{
    local clr="${1}"
    shift
    printf "[%s] \e[${clr}m%s\e[0m\n" "$(date --rfc-3339='seconds')" "$*" >&2;
    return 0
}

function runTest()
{
    local retVal=0

    if [ -f "$1" ]
    then
        $compiler_path < "$1" &> "$test_logs/$1.log"
        retVal=$?
    else
        infoc 31 "Test file '$1' does not exists."
        retVal=1
    fi

    return $retVal
}

function run_suite()
{
    declare -A failed=()

    # Run tests
    for file in "${!test_files[@]}"
    do
        local retValue=0

        infoc 34 "Testing '$file'"
        runTest "$file"
        retValue=$?
        
        if [ $retValue -eq ${test_files[$file]} ]
        then
            infoc 32 "PASS"
        else
            failed+=(["$file"]=$retValue)
            infoc 33 "FAIL"
        fi
    done

    infoc 39 "Suite finished."
    if [ ${#failed[@]} -gt 0 ]
    then
        infoc 31 "Failed tests: ${#failed[@]} / ${#test_files[@]}"

        for fail in "${!failed[@]}"
        do
            infoc 39 "Test '${fail}' exited with code ${failed["$fail"]}." \
                     "Expected: ${test_files["$fail"]}"
        done
    else
        infoc 32 "All tests passed (${#test_files[@]} / ${#test_files[@]})"
    fi

    failed=()
}

# Logs folder
if [ -d "$test_logs" ]
then
    rm -R "$test_logs.old" &> /dev/null
    mv "$test_logs" "$test_logs.old"
fi

mkdir -p "$test_logs" &> /dev/null

run_suite

exit $?

