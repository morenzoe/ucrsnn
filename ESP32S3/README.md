| Supported Targets | ESP32-S3 |
| ----------------- | ----- | 

# _ESP32S3_

Converted code for ESP32-S3 board with ESP-IDF extension in VS Code.

## Example folder contents

The project **ESP32S3** contains one source file in C language [rsnn.c](main/rsnn.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   ├── rsnn.c
│   └── rsnn.h
└── README.md                  This is the file you are currently reading
```
Additionally, this project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
