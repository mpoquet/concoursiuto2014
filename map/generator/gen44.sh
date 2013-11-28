#!/bin/bash

# Generate 4 galaxies of equal size at equal distance with 16 planets in each
./a.out galaxy1.ga 1 0 0 50 4 16 2
./a.out galaxy2.ga 2 0 250 50 4 16 2
./a.out galaxy3.ga 3 250 250 50 4 16 2
./a.out galaxy4.ga 4 250 0 50 4 16 2

cat galaxy1.ga > test.map
cat galaxy2.ga >> test.map 
cat galaxy3.ga >> test.map 
cat galaxy4.ga >> test.map 
