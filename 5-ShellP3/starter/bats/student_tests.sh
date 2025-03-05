#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "exit command returns -7" {
    run ./dsh3 <<EOF
exit 
EOF

    # Assertions
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cmdloopreturned-7"
    [ "$stripped_output" = "$expected_output" ]
}

@test "ls | grep \".c\" finds .c files" {
    run ./dsh3 <<EOF
ls | grep ".c"
EOF

    # Assertions
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dragon.c\ndsh_cli.c\ndshlib.c"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
}

@test "ls command lists directory contents" {
    run ./dsh3 <<EOF
ls
EOF

    # Assertions
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="bats\ndragon.c\ndragon.txt\ndsh\ndsh_cli.c\ndshlib.c\ndshlib.h\nmakefile"
    [ "$stripped_output" = "$expected_output" ]

}

@test "empty string return -1" {
    run ./dsh3 <<EOF

EOF

    # Assertions
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cmdloopreturned-1"
    [ "$stripped_output" = "$expected_output" ]
    
}


