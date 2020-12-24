# MTH9815-Final SOA trading system

### How to run
g++ /home/ubuntu/final/main.cpp

./a.out

### Generate data
Generator python pandas

### modification I made:
- instead of using bondhistoridataservice, I create 5 similar class. Each of them is corresponding one kind of output.
- made pv01 as a member object of bond

### modification you need
- change the path in main and guiservice to the holder you want to store those output
- If there's no output of gui, can change 300 to a smaller number
