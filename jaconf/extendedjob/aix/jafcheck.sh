#!/usr/bin/sh

failure() {
    echo "[ERROR] $1" 1>&2
    exit -1
}

if [ $# -ne 3 ]; then
    failure "Usage: $0 file_name file_delete_flag file_wait_time"
fi

file_name="$1"
file_delete_flag=$2
file_wait_time=$3
file_path=`/usr/bin/dirname "$file_name"`

if ! [ -d $file_path ]; then
    failure "Can not access the path '$file_path'"
fi

if [ -e "$file_name" ]; then
    if [ -f "$file_name" ]; then
        echo "the file $file_name exists"
        if [ $file_delete_flag -eq 1 ]; then
            echo "Delete the file $file_name"
            /usr/bin/rm -f "$file_name" || failure "Can not delete the file $file_name"
        fi
        exit 1
    else
        failure "The file $file_name is not a regular file"
    fi
fi

echo "the file $file_name does not exist"
exit 0
