#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neuroix.h"
#include "llama.h"

// --- Helper Internal ---
void nx_apply_config(NxConfig* source, struct llama_model_params* m_params, struct llama_context_params* c_params) {
    if (!source) return;
    
    *c_params = llama_context_default_params();
    c_params->n_ctx = source->context_size;
    c_params->n_threads = source->n_threads > 0 ? source->n_threads : 4;
    c_params->n_threads_batch = c_params->n_threads;
    
    *m_params = llama_model_default_params();
}

// --- Implementasi Kernel ---

void nx_system_init() {
    // Kita nyalakan log sedikit untuk debug
    // llama_log_set(NULL, NULL);
    
    llama_backend_init();
    printf("[KERNEL] Llama Backend Initialized.\n");
}

NxProcess* nx_create_process(const char* model_path, NxConfig* config) {
    printf("[KERNEL] Loading brain from: %s\n", model_path);
    
    NxProcess* proc = (NxProcess*)malloc(sizeof(NxProcess));
    if (!proc) return NULL;

    struct llama_model_params model_params;
    struct llama_context_params ctx_params;
    nx_apply_config(config, &model_params, &ctx_params);

    // 1. Load Model
    proc->backend_model = llama_load_model_from_file(model_path, model_params);
    if (!proc->backend_model) {
        fprintf(stderr, "[ERROR] FATAL: Failed to load model file! Check path or permissions.\n");
        free(proc);
        return NULL;
    }
    
    // DEBUG: Print alamat pointer model saat dibuat
    printf("[DEBUG] Model loaded at memory address: %p\n", proc->backend_model);

    // 2. Create Context
    proc->backend_context = llama_new_context_with_model((struct llama_model *)proc->backend_model, ctx_params);
    if (!proc->backend_context) {
        fprintf(stderr, "[ERROR] Failed to create RAM context!\n");
        llama_free_model((struct llama_model *)proc->backend_model);
        free(proc);
        return NULL;
    }

    proc->pid = 384; 
    strncpy(proc->model_path, model_path, NX_MAX_PATH - 1);
    proc->state = NX_STATE_READY;
    proc->ram_usage_bytes = 0; 
    if (config) proc->config = *config;

    printf("[KERNEL] Brain active. PID: %d\n", proc->pid);
    return proc;
}

void nx_destroy_process(NxProcess* proc) {
    if (proc) {
        if (proc->backend_context) llama_free((struct llama_context *)proc->backend_context);
        if (proc->backend_model) llama_free_model((struct llama_model *)proc->backend_model);
        free(proc);
        printf("[KERNEL] Process destroyed.\n");
    }
}

char* nx_send_command(NxProcess* proc, const char* input_text) {
    if (proc->state != NX_STATE_READY) return strdup("Error: Busy");
    
    // 1. Safety Check Pointer
    if (!proc->backend_model) return strdup("Error: Model pointer is NULL");
    
    struct llama_model* model = (struct llama_model*)proc->backend_model;
    
    // DEBUG: Cek apakah pointer berubah/rusak
    printf("[DEBUG] Accessing model at address: %p\n", (void*)model);

    // 2. SANITY CHECK: Coba baca properti model
    // Jika crash di sini, berarti struct model rusak/invalid (Memory Corruption)
    int n_vocab = llama_n_vocab(model);
    printf("[DEBUG] Model Sanity Check: Vocab size is %d\n", n_vocab);

    printf("[DEBUG] Tokenizing input: '%s' (Len: %lu)\n", input_text, strlen(input_text));

    // 3. Tokenization (Metode Paling Aman)
    // Alokasi buffer statis yang cukup besar
    const int max_tokens = 2048;
    llama_token tokens[2048]; 
    
    // Hitung panjang string secara eksplisit
    int text_len = (int)strlen(input_text);

    // Panggil llama_tokenize
    // Note: add_bos = true (default behaviour)
    int n_tokens = llama_tokenize(model, input_text, text_len, tokens, max_tokens, true, false);

    // Cek hasil
    if (n_tokens < 0) {
        printf("[DEBUG] Tokenization failed with code: %d\n", n_tokens);
        return strdup("Error: Tokenization failed internal error");
    }

    printf("[DEBUG] Tokenization success. Count: %d\n", n_tokens);

    char buffer[1024];
    snprintf(buffer, 1024, "[KERNEL INFO] Model understood your input as %d tokens.\nFirst ID: %d", 
             n_tokens, 
             n_tokens > 0 ? tokens[0] : -1);

    return strdup(buffer);
}

void nx_print_status(NxProcess* proc) {
    if (!proc) return;
    printf("--- [ REAL PROCESS PID: %d ] ---\n", proc->pid);
    printf("State: %d\n", proc->state);
    printf("------------------------------\n");
}