# Neuroix Kernel

A lightweight, process-oriented C kernel for running local language models, powered by `llama.cpp`.

## Overview

Neuroix provides a simplified C-language wrapper around the powerful `llama.cpp` library. It abstracts away the complexities of the C++ backend, allowing you to load, manage, and interact with GGUF-based language models using a clean, resource-oriented API.

The core concept is the "AI Process" (`NxProcess`), which treats each loaded model as an independent process within the kernel. This makes it easy to manage model lifecycles, configure resources, and run inference tasks in a structured way.

## Features

- **Simple C API:** Easy-to-use functions for all core LLM operations.
- **Process-Oriented Management:** Load and unload models from memory, similar to managing OS processes.
- **`llama.cpp` Backend:** Leverages the performance and broad model support of `llama.cpp`.
- **Resource Configuration:** Specify CPU threads and context size for each model.
- **Cross-Platform:** Compiles and runs on any system with `make` and `gcc`.

## Getting Started

Follow these instructions to build and run the Neuroix Kernel on your local machine.

### Prerequisites

- [Git](https://git-scm.com/)
- [Make](https://www.gnu.org/software/make/)
- A C compiler like [GCC](https://gcc.gnu.org/)

### Build Steps

1.  **Clone the Repository**

    This project uses `llama.cpp` as a git submodule. Clone the repository and initialize the submodule in one step:
    ```sh
    git clone --recurse-submodules https://github.com/your-username/neuroix.git
    cd neuroix
    ```

2.  **Build the `llama.cpp` Dependency**

    Neuroix links against the `llama.cpp` static libraries. You must build them first.
    ```sh
    cd lib/llama.cpp
    make
    cd ../..
    ```
    *Note: This command builds `llama.cpp` for CPU. For GPU (CUDA, Metal, etc.), please follow the official [llama.cpp build instructions](https://github.com/ggerganov/llama.cpp/blob/master/docs/build.md).*

3.  **Build the Neuroix Kernel**

    With `llama.cpp` built, you can now compile the main Neuroix application.
    ```sh
    make
    ```
    This will create the executable at `bin/neuroix`.

### Running the Application

1.  **Download a Model**

    You need a language model in GGUF format. You can download one from Hugging Face. For a quick start, we recommend TinyLlama.
    
    Create a `models` directory and place the file there.
    ```sh
    mkdir -p models
    # Example: wget -O models/tinyllama-1.1b-chat-v1.0.Q4_0.gguf "https://huggingface.co/TheBloke/TinyLlama-1.1B-Chat-v1.0-GGUF/resolve/main/tinyllama-1.1b-chat-v1.0.Q4_0.gguf"
    ```
    The default example in `src/main.c` uses this model path.

2.  **Run the Kernel**

    Execute the compiled program using the `run` command in the Makefile:
    ```sh
    make run
    ```
    Alternatively, you can run the binary directly:
    ```sh
    ./bin/neuroix
    ```

## API Usage

The API in `include/neuroix.h` is designed for simplicity. Here is a minimal example of how to load a model, send a prompt, and clean up.

```c
#include <stdio.h>
#include <stdlib.h>
#include "neuroix.h"

int main() {
    // Initialize the backend
    nx_system_init();

    // Configure the AI process
    NxConfig config = {
        .context_size = 2048,
        .n_threads = 4,
        .use_gpu = false
    };

    // Create a new process by loading a model
    NxProcess* ai_proc = nx_create_process("models/your-model.gguf", &config);
    if (!ai_proc) {
        return 1; // Failed to load
    }

    // Send a prompt and get a response
    const char* prompt = "What is the capital of France?";
    char* response = nx_send_command(ai_proc, prompt);
    
    printf("User > %s\n", prompt);
    printf("AI   > %s\n", response);

    // Free the memory allocated for the response
    free(response);

    // Unload the model and free resources
    nx_destroy_process(ai_proc);

    return 0;
}
```

## Project Structure

```
.
├── bin/              # Compiled binaries
├── build/            # Object files
├── include/          # Header files for Neuroix
├── lib/
│   └── llama.cpp/    # Git submodule for the core backend
├── models/           # Directory for GGUF model files
├── src/              # Source code for Neuroix
└── tests/            # Test files
```

## License

This project is open source. Please add a `LICENSE` file to define the terms under which it is provided. The `llama.cpp` library has its own [MIT License](https://github.com/ggerganov/llama.cpp/blob/master/LICENSE).
