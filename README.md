# Bare-metal Sobel
Bare-metal C program that parallelizes the Sobel algorithm. The code was written for the [CompSoC platform](http://compsoc.eu/) developed at TU Eindhoven. The platform was simulated on a Xilinx FPGA. This project was done for the Embedded Systems Lab (ESL) course at TU Eindhoven.

CompSoC is a predictable and composable architecture based on RISC-V. The predictability property ensures that CompSoC offers hard real-time guarantees. This is achieved via sequential consistency, i.e. all processors in the system observe the same sequential order of memory operations (reads and writes) as if the operations were executed atomically one at a time.parallel threads/processors see the same order of instructions.

CompSoC ensures composability by being a scalable template. One can change the number of RISC-V processor tiles that are available for developing applications. 

With these concepts in mind, the Sobel algorithm was optimized on the custom CompSoC platform with 3 RISC-V processor tiles which are used to compute steps of the algorithm in parallel.

The Sobel algorithm is a classical computer vision algorithm that is used to detect edges of objects in images. It consists of 4 phases:
1. Greyscale - transform the image to black & white
2. Convolution - apply smoothing kernels to reduce noise in the image
3. Sobel - apply the Sobel kernel to detect edges
4. Overlay - apply the detected edges over the original image
