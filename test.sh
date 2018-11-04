#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" != "$expected" ]; then
        echo "$input = $expected expected, but got $actual"
        exit 1
    fi
}

try 0 '0'
try 42 '42'
try 7 '5+2'
try 21 '5+20-4'
try 41 ' 12 + 34 - 5'
try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'
try 1 '1;1'
try 4 '1;(3+5)/2'
try 0 'a=1;0'
try 1 'a=1;0+1'
try 2 'a=1;a+1'
try 2 'a=b=1;a+b'

echo OK