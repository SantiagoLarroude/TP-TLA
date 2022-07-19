#!/usr/bin/bash

readonly compiler_path="../bin/Debug/texler"

# ['filename']=expected compiler exit code
declare -A test_files=(\
    ["r30.texler"]=0 \
    ["r31.texler"]=0 \
    ["r32.texler"]=0 \
    ["r33.texler"]=0 \
    ["r34.texler"]=0 \
    ["r35.texler"]=0 \
    ["r36.texler"]=0 \
    ["r37.texler"]=0 \
    ["r38.texler"]=0 \
    ["r39.texler"]=0 \
    ["r310.texler"]=0 \
    ["r311.texler"]=0 \
    ["r312.texler"]=0 \
    ["r313.texler"]=1 \
    ["r314.texler"]=1 \
    ["r315.texler"]=0 \
)

declare -A test_files_execution_args=(\
    ["r36.texler"]=3 \
)


declare -A test_files_execution=(\
    ["r30.texler"]="" \
    ["r31.texler"]="stdout" \
    ["r32.texler"]="" \
    ["r33.texler"]="new_r33.txt" \
    ["r34.texler"]="new_r34.txt" \
    ["r35.texler"]="new_r35.txt" \
    ["r36.texler"]="new_r36.txt" \
    ["r37.texler"]="new_r37.txt" \
    ["r38.texler"]="new_r38.txt" \
    ["r39.texler"]="new_r39.txt" \
    ["r310.texler"]="new_r310.txt" \
    ["r311.texler"]="stdout" \
    ["r312.texler"]="stdout" \
    ["r315.texler"]="stdout" \
)

readonly test_logs="logs"
readonly test_results="results"

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

function test_compilation()
{
    local retVal=0

    if [ -f "$1" ]
    then
        $compiler_path "$1" &> "$test_logs/test_$1.log"
        retVal=$?
    else
        infoc 31 "Test file '$1' does not exist."
        retVal=1
    fi

    if [ ! -s "$test_logs/test_$1.log" ]
    then
        rm "$test_logs/test_$1.log"
    fi

    return $retVal
}

function test_execution()
{
    local retVal=0

    local texler_source="$1"
    local test_name="$(basename $1)"
    test_name="${test_name%.*}"

    if [ -f "$texler_source" ]
    then
        if [ ${test_files_execution_args[$texler_source]} ]
        then
            ./$test_name.elf \
                ${test_files_execution_args[$texler_source]} \
                &> "$test_results/test_result_${test_name}.txt"
        else
            ./$test_name.elf \
                &> "$test_results/test_result_${test_name}.txt"
        fi

        if [ "${test_files_execution[$texler_source]}" = "" ] || \
            [ "${test_files_execution[$texler_source]}" = "stdout" ]
        then
            diff \
                "test_expected_${test_name}.txt" \
                "$test_results/test_result_${test_name}.txt" \
                &> /dev/null
            retVal=$?
        else
            diff \
                "${test_files_execution[$texler_source]}" \
                "test_expected_${test_name}.txt" \
                &> /dev/null
            retVal=$?

            mv "${test_files_execution[$texler_source]}" "$test_results/"
        fi
    else
        infoc 31 "Test file '$1' does not exist."
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

        local texler_source="$file"
        local test_name="$(basename $file)"
        test_name="${test_name%.*}"

        infoc 34 "Testing '$file'"
        test_compilation "$file"
        retValue=$?
        
        if [ $retValue -eq ${test_files[$file]} ]
        then
            infoc 32 "Compilation: PASS"
        else
            failed+=(["$file"]=$retValue)
            infoc 33 "Compilation: FAIL"
        fi

        retValue=0
        
        if [ ${test_files[$file]} -eq 0 ]
        then
            test_execution "$file" 
            retValue=$?
            
            if [ $retValue -eq 0 ]
            then
                infoc 32 "Execution: PASS"
            else
                failed+=(["$file"]=$retValue)
                infoc 33 "Execution: FAIL"
            fi

            mv "$test_name.elf" "$test_results/"
        fi

        mv "${test_name}"_*.c "$test_results/" &> /dev/null
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
mkdir -p "$test_results" &> /dev/null

run_suite

exit $?

