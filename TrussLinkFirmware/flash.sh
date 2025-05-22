particle list | egrep "online" | awk '/\[([0-9a-f]+?)\]/{ print $2 }' | cut -c 2-25 | xargs -n1 particle flash
#link_array=("22003a001047363336383437" "340047000747363339343638" "3b0064000f51373331333230" "1e0024000147373334323233" "400022001447363336383438" "410023000f51373331333230" "2d0022001847393035313137" "22001b001747393035313137" "3e002a001547393035313136" "29001d001447393035313136" "280034001847393035313137" "410032000547393035313138" "3a003e001947313037363132" "31002b000d47303338353831" "3c001b000d47303338353831" "390025000447393035313138" "2c003f001547313036303933" "180038000d47313037363132" "3d0037000c47393334363636" "3a0037000947313037363132" "2b001f000b47313037363132" "39001a000d47303338353831" "3b0032001547313036303933" "1c0028000b47313037363132" "2e0019001547313036303933" "320037000847313037363132" "2d0031000a47313037363132" "1d0025000e47313037363132" "33003b001547313036303933" "200021001447313036303933")
#for i in ${!link_array[@]}; do
#  echo "Flashing Link $i with ID ${link_array[$i]}"
#  particle cloud flash ${link_array[$i]} 
#done

# echo Flashing 0
# particle cloud flash 22003a001047363336383437
# echo Finished Flashing 0

# echo Flashing 1
# particle cloud flash 340047000747363339343638
# echo Finished Flashing 1

# echo Flashing 2
# particle cloud flash <add ID>
# echo Finished Flashing 2

# echo Flashing 3
# particle cloud flash 3b0064000f51373331333230
# echo Finished Flashing 2

# echo Flashing 4
# particle cloud flash 400022001447363336383438
# echo Finished Flashing 4

# echo Flashing 5
# particle cloud flash 
# echo Finished Flashing 5

# echo Flashing 6
# particle cloud flash 380033000f47363336383437
# echo Finished Flashing 6

# echo Flashing 7
# particle cloud flash 340047000747363339343638
# echo Finished Flashing 7

# echo Flashing 8
# particle cloud flash 2f003c001847393035313138
# echo Finished Flashing 8

# echo Flashing 10
# particle cloud flash 2d0022001847393035313137
# echo Finished Flashing 10

# echo Flashing 13
# particle cloud flash 29001d001447393035313136
# echo Finished Flashing 13

# echo Flashing 20
# particle cloud flash 2c003f001547313036303933
# echo Finished Flashing 20

# echo Flashing 21
# particle cloud flash 180038000d47313037363132
# echo Finished Flashing 21

# echo Flashing 26
# particle cloud flash 3b0032001547313036303933
# echo Finished Flashing 26
