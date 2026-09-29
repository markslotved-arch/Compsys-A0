# Compsys-A0
# KU-ID
Mark Sztuk Slotved/dbl451, Sally Elisabeth Jørgensen/lnp644, Suyu Jia/pbk422

# How to run
To run this program simply, from the root directory type first:
make (name of the file you want to run)
Proceed to type
./ [the file you make] 20000records.tsv
From here you simply type the input you want

# How to test
First enter the Testfolder directory

paste this shell script to test the speed of the coord_query_naive and KDTREE.
chmod +x coord_speed_test.sh
./coord_speed_test.sh

paste this shell script to test the speed of id_query's
chmod +x id_speed_test.sh
./id_speed_test.sh

paste this shell script to test the correctness of our id search implementations
chmod +x id_correctness_test.sh
./id_correctness_test.sh

paste this shell script to test the correctness of our kdtree or naive correctnes
chmod +x [kdtree/naive]_correctness_test.sh
./[kdtree/naive]_correctness_test.sh


# Link to Github
https://github.com/markslotved-arch/Compsys-A0


