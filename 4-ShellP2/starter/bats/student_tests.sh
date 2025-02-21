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

@test "exit exits with exit code -7" {
    run ./dsh <<EOF                
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cmdloopreturned-7"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "cd empty argument remain at the same spot" {
    run ./dsh <<EOF                
pwd
cd
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="/mnt/c/Users/59201/OneDrive/Desktop/CS283/cs283/4-ShellP2/starter/mnt/c/Users/59201/OneDrive/Desktop/CS283/cs283/4-ShellP2/starter"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "cd valid directory goes into directory" {
    run ./dsh <<EOF                
ls
cd bats
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="batsdragon.cdragon.txtdshdsh_cli.cdshlib.cdshlib.hmakefileshell_roadmap.md/mnt/c/Users/59201/OneDrive/Desktop/CS283/cs283/4-ShellP2/starter/bats"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "cd multiple arguments goes into first directory" {
    run ./dsh <<EOF                
ls
cd bats hats wats thats hats nats wat hots
pwd
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="batsdragon.cdragon.txtdshdsh_cli.cdshlib.cdshlib.hmakefileshell_roadmap.md/mnt/c/Users/59201/OneDrive/Desktop/CS283/cs283/4-ShellP2/starter/bats"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "Over 8 arguments returns an error ERR_CMD_ARGS_BAD        -4" {
    run ./dsh <<EOF                
cd bats hats wats thats hats nats wat hots tots
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cmdloopreturned-4"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "No input raises Warn_NO_CMDS -1 error" {
    run ./dsh <<EOF                

EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cmdloopreturned-1"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

@test "Random Inputs does nothing" {
    run ./dsh <<EOF                
a
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output=""
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]    
}

