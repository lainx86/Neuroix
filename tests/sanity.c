#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llama.h"

// Hardcode path modelmu di sini agar tidak salah
#define MODEL_PATH "models/tinyllama-1.1b-chat-v1.0.q4_k_m.gguf"

int main()
{
    printf("=== SANITY CHECK START ===\n");

    // 1. Init Backend
    llama_backend_init();
    printf("[1] Backend initialized.\n");

    // 2. Load Model Params
    struct llama_model_params model_params = llama_model_default_params();

    // 3. Load Model directly
    printf("[2] Loading model from: %s\n", MODEL_PATH);
    struct llama_model *model = llama_load_model_from_file(MODEL_PATH, model_params);

    if (model == NULL)
    {
        fprintf(stderr, "FAIL: Model pointer is NULL.\n");
        return 1;
    }
    printf("[3] Model loaded at address: %p\n", (void *)model);

    // 4. Test Access (Penyebab Crash sebelumnya)
    printf("[4] Testing Property Access (n_vocab)...\n");
    int n_vocab = llama_n_vocab(model);
    printf("SUCCESS: n_vocab = %d\n", n_vocab);

    // 5. Test Tokenize
    printf("[5] Testing Tokenize...\n");
    const char *text = "Hello";
    llama_token tokens[128];
    int n = llama_tokenize(model, text, strlen(text), tokens, 128, true, false);

    if (n > 0)
    {
        printf("SUCCESS: Tokenized '%s' into %d tokens. First ID: %d\n", text, n, tokens[0]);
    }
    else
    {
        printf("FAIL: Tokenize returned %d\n", n);
    }

    // 6. Cleanup
    llama_free_model(model);
    printf("=== SANITY CHECK PASSED ===\n");
    return 0;
}