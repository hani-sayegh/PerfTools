# PerfTools

This repository is designed for **testing the maximum READ (LOAD) memory bandwidth** across different levels of the memory hierarchy, starting from the **L1 cache** up to **1GB** of memory. It serves as a performance benchmarking tool to analyze how efficiently a system handles read operations at varying memory levels.

## Features
- Measures read bandwidth from:
- **L1 Cache**
- **L2 Cache**
- **L3 Cache**
- **Main Memory** (up to 1GB)
- Assumes a maximum SIMD register size of **256 bits** (32 bytes, modern machines probably have 64 bytes but this is the max on my machine).
- Useful for understanding hardware capabilities.

## Requirements
- To Compile:
cl -std:c++17  -O2 -Zi -W4 -nologo {PUT_PATH_DIR_HERE}/main.cpp -Femain_release.exe
If you would like to update the asm file you will need nasm: https://www.nasm.us/
- Then: nasm.exe -W+all -f win64 ./max_bandwidth.asm

- Run the benchmark tool:
 ./perf_tool

- Sample output

```
Memory(KB), Bandwidth (GB/S)
1024, 206.344501
2048, 206.375518
4096, 204.533777
8192, 206.359241
16384, 205.839514
32768, 194.753109
65536, 86.926601
131072, 84.290977
262144, 80.411197
524288, 51.470937
1048576, 50.417173
2097152, 50.730443
4194304, 45.924651
8388608, 17.214537
16777216, 14.260964
33554432, 13.090708
67108864, 12.561495
134217728, 12.419214
268435456, 12.275106
536870912, 12.412826
1073741824, 12.374717
```

## Contributions
Feel free to open issues or submit pull requests for enhancements or bug fixes.

## License
This project is licensed under the MIT License.

---

For any questions or suggestions, please reach out via the Issues section.