# Return -1 if less than, 1 if greater than, 0 if equal.
# Example:
#   val="4.10.0"
#   if [[ $(compare_versions "$val" "4.3.3") -eq -1 ]]; then ...
compare_versions() {
    local IFS='.'
    read -ra v1 <<< "$1"
    read -ra v2 <<< "$2"
    
    for ((i=0; i<${#v1[@]} || i<${#v2[@]}; i++)); do
        (( ${v1[i]:-0} < ${v2[i]:-0} )) && { echo "-1"; return; }
        (( ${v1[i]:-0} > ${v2[i]:-0} )) && { echo "1"; return; }
    done
    echo "0"
}
