# SHA-1 Implementation in C

A bare-metal, strictly mathematically compliant implementation of the SHA-1 hashing algorithm written from scratch in C. 

This project is built to be architecture-independent, ensuring consistent Big-Endian byte parsing and padding across both x86 and ARM systems without relying on external cryptographic libraries. 

Key Features:
Direct Hashing Mode:** Quickly computes the 160-bit message digest for any standard input.
Interactive Tracing Mode:Steps through the internal state of the algorithm, visualizing 512-bit block division, memory-safe message padding, 80-word schedule expansion, and the block compression loop.
Memory Safe:Implements robust standard I/O buffer clearing and leak-proof dynamic reallocation.
