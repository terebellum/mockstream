# mockstream

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
It is a header-only library, you can just copy it into your project, install using cmake, or add_subdirectory using cmake.