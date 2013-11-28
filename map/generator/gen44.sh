#!/bin/bash

# Generate 4 galaxies of equal size at equal distance with 16 planets in each
./a.out galaxy1.ga 1 0 0 4 64 50 4
./a.out galaxy2.ga 2 0 500 4 64 50 4
./a.out galaxy3.ga 3 500 500 4 64 50 4
./a.out galaxy4.ga 4 500 0 4 64 50 4

cat galaxy1.ga > test.map
cat galaxy2.ga >> test.map 
cat galaxy3.ga >> test.map 
cat galaxy4.ga >> test.map 
