# mockstream

[![Release](https://img.shields.io/github/release/terebellum/mockstream.svg)](https://github.com/terebellum/mockstream/releases/latest)

A C++ advanced stream simulation library that brings your test environment closer to production by approximating data flows with math.

It tells **how much** data to send and **when** to send it to pass for the real thing.

## Features

- Header-only library with no external dependencies beyond the C++ Standard Library.

-  **2** different dataflow approximation models: generate data using fixed intervals, probabilistic distributions, and other mathematical models.

- ParameterEstimators: Analyze existing data to extract parameters for any model offline or online.

- All important functions and models annotated with time and memory compexity.

- It is extensible. Easily add new models by implementing a common base.

## Examples

### Basic example

```cpp
#include <mockstream/models/fixed.h>

using namespace mockstream;

int main() {
    // Create fixed model with
    //      interval = 5
    //      amount   = 10
    // 
    // It creates timestamps with intervals of 5 seconds and amount 10
    FixedModelParameters params{5, 10};
    FixedModel model(params);


    while(true) {
        // point has two fields
        //      .timestamp (uint64_t) - timestamp
        //      .amount    (uint64_t) - size of message
        StreamPoint point = model.next();
    
        // Process point
    }
}
```


### Simple http mock
```cpp
#include <mockstream/models/fixed.h>

using namespace mockstream;

// Basic functions of our http mock
Message GenerateMessage(uint64_t size);
void SendMessage(Message message);

int main() {
    // Create fixed model with
    //      interval = 5
    //      amount   = 10
    FixedModelParameters params{5, 10};
    FixedModel model(params);

    // Get first timestamp
    StreamPoint point = model.next();
    SendMessage(GenerateMessage(point.amount));
    while(true) {
        uint64_t last_timestamp = point.timestamp;
        point = model.next();

        // Sleep for interval between timestamps
        std::this_thread::sleep_for(std::chrono::seconds(point.timestamp - last_timestamp));

        SendMessage(GenerateMessage(point.amount));
    }
}
```



## Requirements
* C++17
* CMake (optional)

## Installation
It is a header-only library, you can just copy `include` folder into your project. 

There are also differnet ways of installation using cmake. Library provides `mockstream::mockstream` library, just link it to your target:
```cmake
    target_link_libraries(main PRIVATE mockstream::mockstream)
```

Let's say you cloned mockstream repo:

### 1 Way: Using add_subdirectory

Clone mockstream repo and specify path to it in your cmake

```cmake
    add_subdirectory(lib/mockstream)
```

### 2 Way: Install and use find_package

```bash
    cd /path/to/mockstream
    mkdir build
    cmake ..
    sudo make install
```

In your cmake
```cmake
    find_package(mockstream)
```


### 3 Way: Using Fetchcontent

Just add to your cmake:
```
include(FetchContent)

FetchContent_Declare(mockstream
                     GIT_REPOSITORY https://github.com/terebellum/mockstream
                     GIT_TAG        v0.1.0) # find right version in releases tab
                    
FetchContent_MakeAvailable(mockstream)
```
