database -open -shm waves.shm
probe -create -database waves.shm top -depth all -all -memories
run 1000ns
exit
